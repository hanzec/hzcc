###############################################################################
## BaseParser #################################################################
###############################################################################
file(GLOB MYCC_SYNTAX_BASE_PARSER_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB MYCC_SYNTAX_BASE_PARSER_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_SYNTAX_BASE_PARSER_HDRS} ${MYCC_SYNTAX_BASE_PARSER_SRCS})

###############################################################################
## Statements #################################################################
###############################################################################
file(GLOB MYCC_SYNTAX_STMT_PARSER_HDRS ${CMAKE_CURRENT_LIST_DIR}/statements/*.h)
file(GLOB MYCC_SYNTAX_STMT_PARSER_SRCS ${CMAKE_CURRENT_LIST_DIR}/statements/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_SYNTAX_STMT_PARSER_SRCS} ${MYCC_SYNTAX_STMT_PARSER_HDRS})

###############################################################################
## Other Stuffs ###############################################################
###############################################################################
file(GLOB MYCC_SYNTAX_PARSER_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB MYCC_SYNTAX_PARSER_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_SYNTAX_PARSER_HDRS} ${MYCC_SYNTAX_PARSER_SRCS})

###############################################################################
## Testing Source #############################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB MYCC_SYNTAX_PARSER_TEST_SRCS ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(mycc_test
            PRIVATE
            ${MYCC_SYNTAX_PARSER_TEST_SRCS}
            ${MYCC_SYNTAX_PARSER_HDRS} ${MYCC_SYNTAX_PARSER_SRCS}
            ${MYCC_SYNTAX_BASE_PARSER_HDRS} ${MYCC_SYNTAX_BASE_PARSER_SRCS}
            ${MYCC_SYNTAX_STMT_PARSER_SRCS} ${MYCC_SYNTAX_STMT_PARSER_HDRS})
endif ()