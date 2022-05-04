###############################################################################
## Document Requirements ######################################################
###############################################################################
find_package(LATEX)
find_package(Doxygen)

if (LATEX_FOUND)
    if (LUALATEX__FOUND)
        MESSAGE(STATUS "lualatex found - ${LUALATEX_COMPILER}")
    endif ()

    if (BIBTEX_FOUND)
        MESSAGE(STATUS "bibtex found - ${BIBTEX_COMPILER}")
    endif ()

    if (MAKEINDEX_FOUND)
        MESSAGE(STATUS "makeindex found - ${MAKEINDEX_COMPILER}")
    endif ()
endif ()

###############################################################################
## Latex Sources ##############################################################
###############################################################################
# set latex env
set(TEXMFHOME ${CMAKE_CURRENT_BINARY_DIR})

# Latex source file.
set(DOC_MAIN_TEX "${CMAKE_CURRENT_SOURCE_DIR}/developers.tex")

###############################################################################
## Doxygen Targets ############################################################
###############################################################################
add_custom_target(doc_doxygen)

## if no doxygen found, we install the doxygen from pre-compiled binaries
if (NOT DOXYGEN_FOUND)
    if (UNIX AND NOT MACOSX)
        if (NOT EXISTS ${PROJECT_BINARY_DIR}/tools/doxygen/doxygen-1.9.3/bin)
            MESSAGE(STATUS "Doxygen NOT found. trying to download...")

            ## download doxygen and extract it
            file(DOWNLOAD https://www.doxygen.nl/files/doxygen-1.9.3.linux.bin.tar.gz ${PROJECT_BINARY_DIR}/doxygen.tar.gz)
            file(SHA256 ${PROJECT_BINARY_DIR}/doxygen.tar.gz "44df7b2d139eebb1017c7d6ec8c97082527d84a957c570c94c1301ee8eb4e1ec")
            file(ARCHIVE_EXTRACT INPUT ${PROJECT_BINARY_DIR}/doxygen.tar.gz DESTINATION ${PROJECT_BINARY_DIR}/tools/doxygen)
        else ()
            MESSAGE(STATUS "Found downloaded Doxygen at: ${PROJECT_BINARY_DIR}/tools/doxygen/doxygen-1.9.3/bin")
        endif ()

        ## set true to use the downloaded doxygen
        set(DOXYGEN_FOUND true)

        ## add doxygen to the path
        set(DOXYGEN_PATH ${PROJECT_BINARY_DIR}/tools/doxygen/doxygen-1.9.3/bin)
        set(DOXYGEN_EXECUTABLE ${DOXYGEN_PATH}/doxygen)
    else ()
        message(FATAL_ERROR "Doxygen not found")
    endif ()
endif ()

# set input and output files
set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/Doxyfile)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# generate doxygen configuration file
configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

# note the option ALL which allows to build the docs together with the application
add_custom_command(
        TARGET doc_doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

###############################################################################
## Latex Targets ##############################################################
###############################################################################
if (LATEX_FOUND AND LATEX_PDFLATEX_FOUND AND DOXYGEN_FOUND)
    # copy the figure files to the build directory
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/figure)

    # copy the figure files to the build directory
    file(GLOB LATEX_DOC_FIGURES "${CMAKE_CURRENT_SOURCE_DIR}/figure/*.png")
    foreach (LATEX_DOC_FIGURE ${LATEX_DOC_FIGURES})
        configure_file(${LATEX_DOC_FIGURE} "${CMAKE_CURRENT_BINARY_DIR}/figure/" COPYONLY)
    endforeach ()

    # copy the latex files to the build directory
    file(GLOB LATEX_DOC_FIGURES "${CMAKE_CURRENT_SOURCE_DIR}/*.tex")
    foreach (LATEX_DOC_FIGURE ${LATEX_DOC_FIGURES})
        configure_file(${LATEX_DOC_FIGURE} "${CMAKE_CURRENT_BINARY_DIR}/" COPYONLY)
    endforeach ()

    # copy the sty files to the build directory
    file(GLOB LATEX_DOC_FIGURES "${CMAKE_CURRENT_SOURCE_DIR}/*.sty")
    foreach (LATEX_DOC_FIGURE ${LATEX_DOC_FIGURES})
        configure_file(${LATEX_DOC_FIGURE} "${CMAKE_CURRENT_BINARY_DIR}/" COPYONLY)
    endforeach ()

    # copy the def files to the build directory
    file(GLOB LATEX_DOC_FIGURES "${CMAKE_CURRENT_SOURCE_DIR}/*.def")
    foreach (LATEX_DOC_FIGURE ${LATEX_DOC_FIGURES})
        configure_file(${LATEX_DOC_FIGURE} "${CMAKE_CURRENT_BINARY_DIR}/" COPYONLY)
    endforeach ()

    # Run the latex compiler with draft mode increasing the speed.
    string(REGEX REPLACE "[.]tex$" ".aux" LATEX_AUX_FILES ${DOC_MAIN_TEX})
    get_filename_component(LATEX_AUX_FILES ${LATEX_AUX_FILES} NAME)
    get_filename_component(LATEX_AUX_FILES ${CMAKE_CURRENT_BINARY_DIR}/${LATEX_AUX_FILES} ABSOLUTE)
    add_custom_target(latex-prebuild
            DEPENDS ${DOC_MAIN_TEX}
            BYPRODUCTS ${LATEX_AUX_FILES}
            COMMAND ${CMAKE_COMMAND} -E remove -f *.aux
            COMMAND ${CMAKE_COMMAND} -E remove -f *.idx
            COMMAND ${CMAKE_COMMAND} -E remove -f *.log
            COMMAND ${PDFLATEX_COMPILER} -output-directory ${CMAKE_CURRENT_BINARY_DIR} -draftmode -interaction=nonstopmode -halt-on-error -file-line-error ${DOC_MAIN_TEX} > ${CMAKE_CURRENT_BINARY_DIR}/latex_prebuild.log 2>&1
            COMMENT "Starting Prebuild."
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    # build the latex document are required doxygen documents
    add_dependencies(latex-prebuild doc_doxygen)

    # Generate the indices for the index table.
    string(REGEX REPLACE "[.]tex$" ".ind" LATEX_IND_FILES ${DOC_MAIN_TEX})
    get_filename_component(LATEX_IND_FILES ${LATEX_IND_FILES} NAME)
    get_filename_component(LATEX_IND_FILES ${CMAKE_CURRENT_BINARY_DIR}/${LATEX_IND_FILES} ABSOLUTE)
    add_custom_target(latex-makeindex
            DEPENDS ${DOC_MAIN_TEX} ${LATEX_AUX_FILES}
            BYPRODUCTS ${LATEX_IND_FILES}
            COMMAND ${CMAKE_COMMAND} -E env TEXMFOUTPUT=${CMAKE_CURRENT_BINARY_DIR} ${MAKEINDEX_COMPILER} ${DOC_MAIN_TEX} -o ${LATEX_IND_FILES} -t ${CMAKE_CURRENT_BINARY_DIR}/makeindex.log
            COMMENT "Read and create indices with ${DOC_MAIN_TEX}."
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    # Generate the PDF file.
    string(REGEX REPLACE "[.]tex$" ".pdf" LATEX_PDF_FILES ${DOC_MAIN_TEX})
    get_filename_component(LATEX_PDF_FILES ${LATEX_PDF_FILES} NAME)
    get_filename_component(LATEX_PDF_FILES ${CMAKE_CURRENT_BINARY_DIR}/${LATEX_PDF_FILES} ABSOLUTE)
    add_custom_target(documentation
            DEPENDS ${DOC_MAIN_TEX} ${LATEX_IND_FILES} ${LATEX_AUX_FILES}
            BYPRODUCTS ${LATEX_PDF_FILES}
            COMMAND ${PDFLATEX_COMPILER} -output-directory ${CMAKE_CURRENT_BINARY_DIR} ${DOC_MAIN_TEX} > ${CMAKE_CURRENT_BINARY_DIR}/pdflatex.log 2>&1 && cp ${CMAKE_CURRENT_BINARY_DIR}/*.pdf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMENT "Generating pdf file."
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    add_dependencies(documentation latex-prebuild latex-makeindex)
ELSE ()
    if (NOT LATEX_FOUND)
        message(FATAL_ERROR "Latex is required to build the documentation.")
    else ()
        message(FATAL_ERROR "doxygen is required to build the documentation.")
    endif ()
endif ()
