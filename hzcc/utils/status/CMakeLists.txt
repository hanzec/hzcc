###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_status_utils_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_status_utils_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

file(GLOB hzcc_status_utils_internal_hdrs ${CMAKE_CURRENT_LIST_DIR}/internal/*.h)
file(GLOB hzcc_status_utils_internal_srcs ${CMAKE_CURRENT_LIST_DIR}/internal/*.cc)

# project sources
target_sources(hzcc PRIVATE
        ${hzcc_status_utils_hdrs} ${hzcc_status_utils_srcs}
        ${hzcc_status_utils_internal_hdrs} ${hzcc_status_utils_internal_srcs})

###############################################################################
## Testing hzcc #############################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    file(GLOB hzcc_utils_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    target_sources(hzcc_test
            PRIVATE
            ${hzcc_utils_test}
            ${hzcc_status_utils_hdrs} ${hzcc_status_utils_srcs}
            ${hzcc_status_utils_internal_hdrs} ${hzcc_status_utils_internal_srcs})
endif ()
