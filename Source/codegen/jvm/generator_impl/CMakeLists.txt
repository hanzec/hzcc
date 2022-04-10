###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB MYCC_CODEGEN_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB MYCC_CODEGEN_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_CODEGEN_HDRS} ${MYCC_CODEGEN_SRCS})

###############################################################################
## AST Cast ###################################################################
###############################################################################
file(GLOB MYCC_CODEGEN_CAST_HDRS expr/cast/*.h)
file(GLOB MYCC_CODEGEN_CAST_SRCS expr/cast/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_CODEGEN_CAST_HDRS} ${MYCC_CODEGEN_CAST_SRCS})

###############################################################################
## AST Type ###################################################################
###############################################################################
file(GLOB MYCC_CODEGEN_TYPE_HDRS ${CMAKE_CURRENT_LIST_DIR}/type/*.h)
file(GLOB MYCC_CODEGEN_TYPE_SRCS ${CMAKE_CURRENT_LIST_DIR}/type/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_CODEGEN_TYPE_HDRS} ${MYCC_CODEGEN_TYPE_SRCS})

###############################################################################
## AST Utils ##################################################################
###############################################################################
file(GLOB MYCC_CODEGEN_UTILS_HDRS ${CMAKE_CURRENT_LIST_DIR}/utils/*.h)
file(GLOB MYCC_CODEGEN_UTILS_SRCS ${CMAKE_CURRENT_LIST_DIR}/utils/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_CODEGEN_UTILS_HDRS} ${MYCC_CODEGEN_UTILS_SRCS})

###############################################################################
## AST Value ##################################################################
###############################################################################
file(GLOB MYCC_CODEGEN_VALUE_HDRS ${CMAKE_CURRENT_LIST_DIR}/value/*.h)
file(GLOB MYCC_CODEGEN_VALUE_SRCS ${CMAKE_CURRENT_LIST_DIR}/value/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_CODEGEN_VALUE_HDRS} ${MYCC_CODEGEN_VALUE_SRCS})

###############################################################################
## AST Operator #############################################################
###############################################################################
file(GLOB MYCC_CODEGEN_EXPRESSION_HDRS ${CMAKE_CURRENT_LIST_DIR}/expr/*.h)
file(GLOB MYCC_CODEGEN_EXPRESSION_SRCS ${CMAKE_CURRENT_LIST_DIR}/expr/*.cc)
file(GLOB MYCC_CODEGEN_EXPRESSION_OP_HDRS ${CMAKE_CURRENT_LIST_DIR}/expr/operator/*.h)
file(GLOB MYCC_CODEGEN_EXPRESSION_OP_SRCS ${CMAKE_CURRENT_LIST_DIR}/expr/operator/*.cc)
# project sources
target_sources(mycc PRIVATE
        ${MYCC_CODEGEN_EXPRESSION_HDRS} ${MYCC_CODEGEN_EXPRESSION_SRCS}
        ${MYCC_CODEGEN_EXPRESSION_OP_HDRS} ${MYCC_CODEGEN_EXPRESSION_OP_SRCS})

###############################################################################
## AST Statement #############################################################
###############################################################################
file(GLOB MYCC_CODEGEN_STATEMENTS_HDRS ${CMAKE_CURRENT_LIST_DIR}/statement/*.h)
file(GLOB MYCC_CODEGEN_STATEMENTS_SRCS ${CMAKE_CURRENT_LIST_DIR}/statement/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_CODEGEN_STATEMENTS_HDRS} ${MYCC_CODEGEN_STATEMENTS_SRCS})

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
            ${MYCC_CODEGEN_HDRS} ${MYCC_CODEGEN_SRCS}
            ${MYCC_CODEGEN_CAST_HDRS} ${MYCC_CODEGEN_CAST_SRCS}
            ${MYCC_CODEGEN_TYPE_HDRS} ${MYCC_CODEGEN_TYPE_SRCS}
            ${MYCC_CODEGEN_UTILS_HDRS} ${MYCC_CODEGEN_UTILS_SRCS}
            ${MYCC_CODEGEN_VALUE_HDRS} ${MYCC_CODEGEN_VALUE_SRCS}
            ${MYCC_CODEGEN_STATEMENTS_HDRS} ${MYCC_CODEGEN_STATEMENTS_SRCS}
            ${MYCC_CODEGEN_EXPRESSION_HDRS} ${MYCC_CODEGEN_EXPRESSION_SRCS}
            ${MYCC_CODEGEN_EXPRESSION_OP_HDRS} ${MYCC_CODEGEN_EXPRESSION_OP_SRCS})
endif ()