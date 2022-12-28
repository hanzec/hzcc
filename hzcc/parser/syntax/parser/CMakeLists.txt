###############################################################################
## BaseParser #################################################################
###############################################################################
file(GLOB hzcc_syntax_parser_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_syntax_parser_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_syntax_parser_hdrs} ${hzcc_syntax_parser_srcs})

###############################################################################
## Statements #################################################################
###############################################################################
file(GLOB hzcc_syntax_parser_stmt_hdrs ${CMAKE_CURRENT_LIST_DIR}/statements/*.h)
file(GLOB hzcc_syntax_parser_stmt_srcs ${CMAKE_CURRENT_LIST_DIR}/statements/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_syntax_parser_stmt_srcs} ${hzcc_syntax_parser_stmt_hdrs})

###############################################################################
## parser Utils ###############################################################
###############################################################################
file(GLOB hzcc_syntax_parser_utils_hdrs ${CMAKE_CURRENT_LIST_DIR}/utils/*.h)
file(GLOB hzcc_syntax_parser_utils_srcs ${CMAKE_CURRENT_LIST_DIR}/utils/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_syntax_parser_utils_hdrs} ${hzcc_syntax_parser_utils_srcs})

###############################################################################
## Testing hzcc #############################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_syntax_parser_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(hzcc_test
            PRIVATE
            ${hzcc_syntax_parser_test}
            ${hzcc_syntax_parser_hdrs} ${hzcc_syntax_parser_srcs}
            ${hzcc_syntax_parser_stmt_srcs} ${hzcc_syntax_parser_stmt_hdrs}
            ${hzcc_syntax_parser_utils_hdrs} ${hzcc_syntax_parser_utils_srcs})
endif ()
