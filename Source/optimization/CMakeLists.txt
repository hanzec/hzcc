###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB optimization_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB optimization_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(mycc PRIVATE ${optimization_hdrs} ${optimization_srcs})

###############################################################################
## Passes #####################################################################
###############################################################################
file(GLOB optimization_pass_hdrs ${CMAKE_CURRENT_LIST_DIR}/pass/*.h)
file(GLOB optimization_pass_srcs ${CMAKE_CURRENT_LIST_DIR}/pass/*.cc)
file(GLOB optimization_pass_impl_hdrs ${CMAKE_CURRENT_LIST_DIR}/pass_impl/*.h)
file(GLOB optimization_pass_impl_srcs ${CMAKE_CURRENT_LIST_DIR}/pass_impl/*.cc)

# project sources
target_sources(mycc PRIVATE
        ${optimization_pass_hdrs} ${optimization_pass_srcs}
        ${optimization_pass_impl_hdrs} ${optimization_pass_impl_srcs})

###############################################################################
## Testing Source #############################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB optimization_tests ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(mycc_test
            PRIVATE
            ${optimization_tests}
            ${optimization_hdrs} ${optimization_srcs}
            ${optimization_pass_hdrs} ${optimization_pass_srcs})
endif ()
