# Documentation is opt-in at the project root. Use installed tools only.
find_package(Doxygen REQUIRED OPTIONAL_COMPONENTS dot)
find_package(LATEX QUIET COMPONENTS PDFLATEX MAKEINDEX)

set(HZCC_DOC_GENERATE_LATEX NO)
if (LATEX_PDFLATEX_FOUND AND LATEX_MAKEINDEX_FOUND)
    set(HZCC_DOC_GENERATE_LATEX YES)
else ()
    message(STATUS
        "pdflatex and/or makeindex not found: documentation will generate HTML only")
endif ()

set(HZCC_DOC_HAVE_DOT NO)
if (DOXYGEN_DOT_FOUND)
    set(HZCC_DOC_HAVE_DOT YES)
endif ()

# developers.tex embeds refman.tex, so Doxygen must omit its document wrapper.
# An empty file is portable, unlike the previous /dev/null configuration.
# Keep generated API files separate from Doxygen configuration and manual inputs.
set(HZCC_DOC_LATEX_DIR "${CMAKE_CURRENT_BINARY_DIR}/latex")
set(HZCC_DOC_EMPTY_TEX "${CMAKE_CURRENT_BINARY_DIR}/empty.tex")
file(WRITE "${HZCC_DOC_EMPTY_TEX}" "")
configure_file("${PROJECT_SOURCE_DIR}/Doxyfile"
               "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

add_custom_target(doc_doxygen
    # A failed extraction must not leave an older XML index available for audit.
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${PROJECT_BINARY_DIR}/doxygen_xml"
    COMMAND Doxygen::doxygen "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)

if (HZCC_DOC_GENERATE_LATEX STREQUAL "YES")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/developers.tex"
                   "${CMAKE_CURRENT_BINARY_DIR}/developers.tex" COPYONLY)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/figure")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/figure/frontend_structure.png"
                   "${CMAKE_CURRENT_BINARY_DIR}/figure/frontend_structure.png" COPYONLY)

    # Search generated API inputs while leaving the manual and PDF in doc/.
    # A trailing separator retains TeX's default package search paths.
    if (WIN32)
        set(HZCC_TEXINPUTS "${HZCC_DOC_LATEX_DIR};.;$ENV{TEXINPUTS};")
        string(REPLACE ";" "\\;" HZCC_TEXINPUTS "${HZCC_TEXINPUTS}")
    else ()
        set(HZCC_TEXINPUTS "${HZCC_DOC_LATEX_DIR}:.:$ENV{TEXINPUTS}:")
    endif ()
    set(HZCC_PDFLATEX_COMMAND "${CMAKE_COMMAND}" -E env
        "TEXINPUTS=${HZCC_TEXINPUTS}" "${PDFLATEX_COMPILER}")
    add_custom_target(documentation
        COMMAND ${HZCC_PDFLATEX_COMMAND} -interaction=nonstopmode -halt-on-error
                -file-line-error developers.tex
        COMMAND "${MAKEINDEX_COMPILER}" developers.idx
        COMMAND ${HZCC_PDFLATEX_COMMAND} -interaction=nonstopmode -halt-on-error
                -file-line-error developers.tex
        COMMAND ${HZCC_PDFLATEX_COMMAND} -interaction=nonstopmode -halt-on-error
                -file-line-error developers.tex
        BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/developers.pdf"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Generating the developer manual PDF"
        VERBATIM)
else ()
    add_custom_target(documentation
        COMMAND "${CMAKE_COMMAND}" -E echo
                "HTML documentation: ${PROJECT_BINARY_DIR}/doxygen_html/index.html (PDF requires pdflatex and makeindex)"
        VERBATIM)
endif ()
add_dependencies(documentation doc_doxygen)

# Audit the XML, including private and experimental interfaces, independently of
# presentation-only warnings about fields or enum values.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_custom_target(check_doxygen
    COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/scripts/check_doxygen.py"
            "${PROJECT_BINARY_DIR}/doxygen_xml"
    DEPENDS doc_doxygen
    COMMENT "Checking Doxygen coverage for project classes and functions"
    VERBATIM)
