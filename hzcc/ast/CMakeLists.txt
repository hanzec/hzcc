###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_ast_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_ast_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_ast_hdrs} ${hzcc_ast_srcs})

###############################################################################
## ast Cast ###################################################################
###############################################################################
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/cast)

###############################################################################
## ast type ###################################################################
###############################################################################
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/type)

###############################################################################
## ast Expr ##################################################################
###############################################################################
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/expr)

###############################################################################
## ast utils ##################################################################
###############################################################################
file(GLOB hzcc_ast_utils_hdrs ${CMAKE_CURRENT_LIST_DIR}/utils/*.h)
file(GLOB hzcc_ast_utils_srcs ${CMAKE_CURRENT_LIST_DIR}/utils/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_ast_utils_hdrs} ${hzcc_ast_utils_srcs})

###############################################################################
## ast Statement #############################################################
###############################################################################
file(GLOB hzcc_ast_statements_hdrs ${CMAKE_CURRENT_LIST_DIR}/stmt/*.h)
file(GLOB hzcc_ast_statements_srcs ${CMAKE_CURRENT_LIST_DIR}/stmt/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_ast_statements_hdrs} ${hzcc_ast_statements_srcs})

###############################################################################
## Testing hzcc #############################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_SYNTAX_TEST_SRCS ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to hzcc_test target
    target_sources(hzcc_test
            PRIVATE
            ${hzcc_SYNTAX_TEST_SRCS}
            ${hzcc_ast_hdrs} ${hzcc_ast_srcs}
            ${hzcc_ast_cast_hdrs} ${hzcc_ast_cast_srcs}
            ${hzcc_ast_type_hdrs} ${hzcc_ast_type_srcs}
            ${hzcc_ast_utils_hdrs} ${hzcc_ast_utils_srcs}
            ${hzcc_ast_value_hdrs} ${hzcc_ast_value_srcs}
            ${hzcc_ast_utils_hdrs} ${hzcc_ast_utils_srcs}
            ${hzcc_ast_statements_hdrs} ${hzcc_ast_statements_srcs}
            ${hzcc_ast_expression_hdrs} ${hzcc_ast_expression_srcs}
            ${hzcc_ast_expression_op_hdrs} ${hzcc_ast_expression_op_srcs})
endif ()
