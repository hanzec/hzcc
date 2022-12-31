###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_expr_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_expr_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_expr_hdrs} ${hzcc_expr_srcs})

###############################################################################
## Binary Operator Sources ####################################################
###############################################################################
file(GLOB hzcc_cast_biop_hdrs ${CMAKE_CURRENT_LIST_DIR}/biop/*.h)
file(GLOB hzcc_cast_biop_srcs ${CMAKE_CURRENT_LIST_DIR}/biop/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_cast_biop_hdrs} ${hzcc_cast_biop_srcs})

###############################################################################
## Cast Expression ############################################################
###############################################################################
file(GLOB hzcc_cast_cast_expr_hdrs ${CMAKE_CURRENT_LIST_DIR}/cast/*.h)
file(GLOB hzcc_cast_cast_expr_srcs ${CMAKE_CURRENT_LIST_DIR}/cast/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_cast_cast_expr_hdrs} ${hzcc_cast_cast_expr_srcs})

###############################################################################
## Testing hzcc #############################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_test_srcs ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to hzcc_test target
    target_sources(hzcc_test
            PRIVATE
            ${hzcc_test_srcs}
            ${hzcc_expr_hdrs} ${hzcc_expr_srcs}
            ${hzcc_cast_biop_hdrs} ${hzcc_cast_biop_srcs}
            ${hzcc_grammar_expr_hdrs} ${hzcc_grammar_expr_srcs}
            ${hzcc_cast_cast_expr_hdrs} ${hzcc_cast_cast_expr_srcs})
endif ()