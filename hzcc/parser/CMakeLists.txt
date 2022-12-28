###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_parser_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_parser_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_parser_hdrs} ${hzcc_parser_srcs})

###############################################################################
## Parser Common ##############################################################
###############################################################################
add_subdirectory(common)

###############################################################################
## lexical Analysis ###########################################################
###############################################################################
if (ENABLE_ANTLR MATCHES "ON")
    message(STATUS "ANTLR is enabled, will use ANTLR to generate lexer and parser")
else ()
    add_subdirectory(lexical)
endif ()

###############################################################################
## syntax parser ##############################################################
###############################################################################
if (ENABLE_ANTLR MATCHES "ON")
    add_subdirectory(antlr)
else ()
    add_subdirectory(syntax)
endif ()

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
            ${hzcc_parser_hdrs} ${hzcc_parser_srcs}
            ${hzcc_parser_common_hdrs} ${hzcc_parser_common_srcs})
endif ()
