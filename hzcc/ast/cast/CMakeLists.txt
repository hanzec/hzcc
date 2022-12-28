###############################################################################
## Cast Utils Sources #########################################################
###############################################################################
file(GLOB hzcc_cast_cast_utils_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_cast_cast_utils_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_cast_cast_utils_hdrs} ${hzcc_cast_cast_utils_srcs})

###############################################################################
## Cast Expression ############################################################
###############################################################################
file(GLOB hzcc_cast_cast_expr_hdrs ../expr/cast/*.h)
file(GLOB hzcc_cast_cast_expr_srcs ../expr/cast/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_cast_cast_expr_hdrs} ${hzcc_cast_cast_expr_srcs})

###############################################################################
## Cast Rules #################################################################
###############################################################################
file(GLOB hzcc_cast_cast_rule_hdrs ${CMAKE_CURRENT_LIST_DIR}/rule/*.h)
file(GLOB hzcc_cast_cast_rule_srcs ${CMAKE_CURRENT_LIST_DIR}/rule/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_cast_cast_rule_hdrs} ${hzcc_cast_cast_rule_srcs})

###############################################################################
## Cast Expression ############################################################
###############################################################################
file(GLOB hzcc_cast_cast_expr_implicit_hdrs ../expr/cast/implicit_cast/*.h)
file(GLOB hzcc_cast_cast_expr_implicit_srcs ../expr/cast/implicit_cast/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_cast_cast_expr_implicit_hdrs} ${hzcc_cast_cast_expr_implicit_srcs})

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
            ${hzcc_cast_cast_utils_hdrs} ${hzcc_cast_cast_utils_srcs}
            ${hzcc_cast_cast_expr_hdrs} ${hzcc_cast_cast_expr_srcs}
            ${hzcc_cast_cast_rule_hdrs} ${hzcc_cast_cast_rule_srcs}
            ${hzcc_cast_cast_expr_implicit_hdrs} ${hzcc_cast_cast_expr_implicit_srcs})
endif ()