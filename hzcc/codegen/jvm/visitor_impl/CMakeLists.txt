###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_code_gen_visitor_impl_hdr ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_code_gen_visitor_impl_src ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE
        ${hzcc_code_gen_visitor_impl_hdr} ${hzcc_code_gen_visitor_impl_src})

###############################################################################
## ast Cast ###################################################################
###############################################################################
file(GLOB hzcc_code_gen_visitor_impl_cast_hdrs expr/cast/*.h)
file(GLOB hzcc_code_gen_visitor_impl_cast_srcs expr/cast/*.cc)

# project sources
target_sources(hzcc PRIVATE
        ${hzcc_code_gen_visitor_impl_cast_hdrs} ${hzcc_code_gen_visitor_impl_cast_srcs})

###############################################################################
## ast RetType ###################################################################
###############################################################################
file(GLOB hzcc_code_gen_visitor_impl_type_hdrs ${CMAKE_CURRENT_LIST_DIR}/type/*.h)
file(GLOB hzcc_code_gen_visitor_impl_type_srcs ${CMAKE_CURRENT_LIST_DIR}/type/*.cc)

# project sources
target_sources(hzcc PRIVATE
        ${hzcc_code_gen_visitor_impl_type_hdrs} ${hzcc_code_gen_visitor_impl_type_srcs})

###############################################################################
## ast Utils ##################################################################
###############################################################################
file(GLOB hzcc_code_gen_visitor_impl_utils_hdrs ${CMAKE_CURRENT_LIST_DIR}/utils/*.h)
file(GLOB hzcc_code_gen_visitor_impl_utils_srcs ${CMAKE_CURRENT_LIST_DIR}/utils/*.cc)

# project sources
target_sources(hzcc PRIVATE
        ${hzcc_code_gen_visitor_impl_utils_hdrs} ${hzcc_code_gen_visitor_impl_utils_srcs})

###############################################################################
## ast Value ##################################################################
###############################################################################
file(GLOB hzcc_code_gen_visitor_impl_value_hdrs ${CMAKE_CURRENT_LIST_DIR}/value/*.h)
file(GLOB hzcc_code_gen_visitor_impl_value_srcs ${CMAKE_CURRENT_LIST_DIR}/value/*.cc)

# project sources
target_sources(hzcc PRIVATE
        ${hzcc_code_gen_visitor_impl_value_hdrs} ${hzcc_code_gen_visitor_impl_value_srcs})

###############################################################################
## ast Operator #############################################################
###############################################################################
file(GLOB hzcc_code_gen_visitor_impl_expression_hdrs ${CMAKE_CURRENT_LIST_DIR}/expr/*.h)
file(GLOB hzcc_code_gen_visitor_impl_expression_srcs ${CMAKE_CURRENT_LIST_DIR}/expr/*.cc)
file(GLOB hzcc_code_gen_visitor_impl_expression_op_hdrs ${CMAKE_CURRENT_LIST_DIR}/expr/operator/*.h)
file(GLOB hzcc_code_gen_visitor_impl_expression_op_srcs ${CMAKE_CURRENT_LIST_DIR}/expr/operator/*.cc)
# project sources
target_sources(hzcc PRIVATE
        ${hzcc_code_gen_visitor_impl_expression_hdrs} ${hzcc_code_gen_visitor_impl_expression_srcs}
        ${hzcc_code_gen_visitor_impl_expression_op_hdrs} ${hzcc_code_gen_visitor_impl_expression_op_srcs})

###############################################################################
## ast Statement #############################################################
###############################################################################
file(GLOB hzcc_code_gen_visitor_impl_statements_hdrs ${CMAKE_CURRENT_LIST_DIR}/stmt/*.h)
file(GLOB hzcc_code_gen_visitor_impl_statements_srcs ${CMAKE_CURRENT_LIST_DIR}/stmt/*.cc)

# project sources
target_sources(hzcc PRIVATE
        ${hzcc_code_gen_visitor_impl_statements_hdrs} ${hzcc_code_gen_visitor_impl_statements_srcs})

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
            ${hzcc_code_gen_visitor_impl_hdr} ${hzcc_code_gen_visitor_impl_src}
            ${hzcc_code_gen_visitor_impl_cast_hdrs} ${hzcc_code_gen_visitor_impl_cast_srcs}
            ${hzcc_code_gen_visitor_impl_type_hdrs} ${hzcc_code_gen_visitor_impl_type_srcs}
            ${hzcc_code_gen_visitor_impl_utils_hdrs} ${hzcc_code_gen_visitor_impl_utils_srcs}
            ${hzcc_code_gen_visitor_impl_value_hdrs} ${hzcc_code_gen_visitor_impl_value_srcs}
            ${hzcc_code_gen_visitor_impl_statements_hdrs} ${hzcc_code_gen_visitor_impl_statements_srcs}
            ${hzcc_code_gen_visitor_impl_expression_hdrs} ${hzcc_code_gen_visitor_impl_expression_srcs}
            ${hzcc_code_gen_visitor_impl_expression_op_hdrs} ${hzcc_code_gen_visitor_impl_expression_op_srcs})
endif ()
