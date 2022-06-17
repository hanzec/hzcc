###############################################################################
## AST Type ##############################################################
###############################################################################
file(GLOB hzcc_ast_type_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_ast_type_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_ast_type_hdrs} ${hzcc_ast_type_srcs})

###############################################################################
## Testing  ###################################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_syntax_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(hzcc_test PRIVATE
            ${hzcc_syntax_test}
            ${hzcc_ast_type_hdrs} ${hzcc_ast_type_srcs})
endif ()
