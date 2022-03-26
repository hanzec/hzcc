###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB MYCC_AST_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB MYCC_AST_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_AST_HDRS} ${MYCC_AST_SRCS})

###############################################################################
## AST Type ###################################################################
###############################################################################
file(GLOB MYCC_AST_TYPE_HDRS ${CMAKE_CURRENT_LIST_DIR}/type/*.h)
file(GLOB MYCC_AST_TYPE_SRCS ${CMAKE_CURRENT_LIST_DIR}/type/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_AST_TYPE_HDRS} ${MYCC_AST_TYPE_SRCS})

###############################################################################
## AST Value ##################################################################
###############################################################################
file(GLOB MYCC_AST_VALUE_HDRS ${CMAKE_CURRENT_LIST_DIR}/value/*.h)
file(GLOB MYCC_AST_VALUE_SRCS ${CMAKE_CURRENT_LIST_DIR}/value/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_AST_VALUE_HDRS} ${MYCC_AST_VALUE_SRCS})

###############################################################################
## AST Operator #############################################################
###############################################################################
file(GLOB MYCC_AST_EXPRESSION_HDRS ${CMAKE_CURRENT_LIST_DIR}/operator/*.h)
file(GLOB MYCC_AST_EXPRESSION_SRCS ${CMAKE_CURRENT_LIST_DIR}/operator/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_AST_EXPRESSION_HDRS} ${MYCC_AST_EXPRESSION_SRCS})

###############################################################################
## AST Statement #############################################################
###############################################################################
file(GLOB MYCC_AST_STATEMENTS_HDRS ${CMAKE_CURRENT_LIST_DIR}/statement/*.h)
file(GLOB MYCC_AST_STATEMENTS_SRCS ${CMAKE_CURRENT_LIST_DIR}/statement/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_AST_STATEMENTS_HDRS} ${MYCC_AST_STATEMENTS_SRCS})

###############################################################################
## Testing Source #############################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB MYCC_SYNTAX_TEST_SRCS ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(mycc_test
            PRIVATE
            ${MYCC_SYNTAX_TEST_SRCS}
            ${MYCC_AST_HDRS} ${MYCC_AST_SRCS}
            ${MYCC_SYNTAX_PARSER_HDRS} ${MYCC_SYNTAX_PARSER_SRCS})
endif ()