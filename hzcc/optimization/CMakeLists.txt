###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_optimization_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_optimization_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_optimization_hdrs} ${hzcc_optimization_srcs})

###############################################################################
## Passes #####################################################################
###############################################################################
file(GLOB hzcc_optimization_pass_hdrs ${CMAKE_CURRENT_LIST_DIR}/pass/*.h)
file(GLOB hzcc_optimization_pass_srcs ${CMAKE_CURRENT_LIST_DIR}/pass/*.cc)
file(GLOB hzcc_optimization_pass_impl_hdrs ${CMAKE_CURRENT_LIST_DIR}/pass_impl/*.h)
file(GLOB hzcc_optimization_pass_impl_srcs ${CMAKE_CURRENT_LIST_DIR}/pass_impl/*.cc)

# project sources
target_sources(hzcc PRIVATE
        ${hzcc_optimization_pass_hdrs} ${hzcc_optimization_pass_srcs}
        ${hzcc_optimization_pass_impl_hdrs} ${hzcc_optimization_pass_impl_srcs})

###############################################################################
## Testing hzcc #############################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_optimization_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(hzcc_test
            PRIVATE
            ${hzcc_optimization_test}
            ${hzcc_optimization_hdrs} ${hzcc_optimization_srcs}
            ${hzcc_optimization_pass_hdrs} ${hzcc_optimization_pass_srcs})
endif ()
