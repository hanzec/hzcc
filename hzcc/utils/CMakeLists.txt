###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_utils_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_utils_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_utils_hdrs} ${hzcc_utils_srcs})

###############################################################################
## Status Utils ###############################################################
###############################################################################
add_subdirectory(status)

###############################################################################
## Testing hzcc #############################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    file(GLOB hzcc_utils_test status/test/*_test.cc)

    target_sources(hzcc_test
            PRIVATE
            ${hzcc_utils_test}
            ${hzcc_utils_hdrs} ${hzcc_utils_srcs})
endif ()
