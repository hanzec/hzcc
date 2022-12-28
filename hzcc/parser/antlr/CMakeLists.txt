###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_antlr_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_antlr_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_antlr_hdrs} ${hzcc_antlr_srcs})

###############################################################################
## ANTLR Generate #############################################################
###############################################################################
# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(ANTLR4_WITH_STATIC_CRT OFF)
endif ()

# import antlr4
set(ANTLR4_TAG 4.11.1)
message(STATUS "Select antlr4 version: ${ANTLR4_TAG}")

# download antlr4 jar if not exists
if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/antlr-${ANTLR4_TAG}-complete.jar)
    message(STATUS "Download antlr4 jar to ${CMAKE_CURRENT_BINARY_DIR}/antlr-${ANTLR4_TAG}-complete.jar")
    file(DOWNLOAD https://www.antlr.org/download/antlr-${ANTLR4_TAG}-complete.jar
            ${CMAKE_CURRENT_BINARY_DIR}/antlr-${ANTLR4_TAG}-complete.jar
            SHOW_PROGRESS)
endif ()
set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/antlr-${ANTLR4_TAG}-complete.jar)

# find antlr4 jar
find_package(Java REQUIRED)
find_package(ANTLR REQUIRED)
find_package(antlr4-runtime REQUIRED)
target_link_libraries(hzcc antlr4-runtime)

# generate antlr4 lexical and syntax
antlr_target(antlr4_parser
        ${CMAKE_CURRENT_SOURCE_DIR}/c11.g4
        VISITOR
        PACKAGE hzcc::parser
        OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/antlr_parser
        )

# custom target for antlr4
add_custom_target(generate_antlr4
                  DEPENDS ${ANTLR_antlr4_parser_OUTPUTS})

# add antlr4 lexical and syntax to project
add_dependencies(hzcc generate_antlr4)

# add generated antlr4 lexical and syntax to project
target_sources(hzcc PRIVATE ${ANTLR4_SRC_FILES_antlr4_parser})

###############################################################################
## Testing hzcc #############################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_parser_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(hzcc_test
            PRIVATE
            ${hzcc_parser_test}
            ${hzcc_parser_hdrs} ${hzcc_parser_srcs})
endif ()
