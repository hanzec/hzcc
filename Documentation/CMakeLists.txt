###############################################################################
## Latex Requirements #########################################################
###############################################################################
find_package(LATEX)

if(LATEX_FOUND)
    if (LUALATEX__FOUND)
        MESSAGE(STATUS "lualatex found - ${LUALATEX_COMPILER}")
    endif ()

    if (BIBTEX_FOUND)
        MESSAGE(STATUS "bibtex found - ${BIBTEX_COMPILER}")
    endif ()

    if (MAKEINDEX_FOUND)
        MESSAGE(STATUS "makeindex found - ${MAKEINDEX_COMPILER}")
    endif ()
endif()

###############################################################################
## Latex Sources ##############################################################
###############################################################################
# set latex env
set(TEXMFHOME ${CMAKE_CURRENT_BINARY_DIR})

# Latex source file.
set(DOC_MAIN_TEX "${CMAKE_CURRENT_SOURCE_DIR}/developers.tex")

###############################################################################
## Latex Targets ##############################################################
###############################################################################
if (LATEX_FOUND AND LATEX_PDFLATEX_FOUND)
    # Run the latex compiler with draft mode increasing the speed.
    string(REGEX REPLACE "[.]tex$" ".aux" LATEX_AUX_FILES ${DOC_MAIN_TEX})
    get_filename_component(LATEX_AUX_FILES ${LATEX_AUX_FILES} NAME)
    get_filename_component(LATEX_AUX_FILES ${CMAKE_CURRENT_BINARY_DIR}/${LATEX_AUX_FILES} ABSOLUTE)
    add_custom_target(latex-prebuild
            DEPENDS ${DOC_MAIN_TEX}
            BYPRODUCTS ${LATEX_AUX_FILES}
            COMMAND ${PDFLATEX_COMPILER} -output-directory ${CMAKE_CURRENT_BINARY_DIR} -draftmode -interaction=nonstopmode ${DOC_MAIN_TEX}
            COMMENT "Starting Prebuild."
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

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
            COMMAND ${PDFLATEX_COMPILER} -output-directory ${CMAKE_CURRENT_BINARY_DIR} ${DOC_MAIN_TEX}
            COMMENT "Generating pdf file."
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    add_dependencies(documentation latex-prebuild latex-makeindex)

    ADD_CUSTOM_TARGET(all-formats ALL) # Entry point of execution.
    ADD_DEPENDENCIES(all-formats documentation)
ELSE ()
    MESSAGE(ERROR "No latex tools found!")
endif ()