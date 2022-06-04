###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_lexical_utils_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_lexical_utils_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_lexical_utils_hdrs} ${hzcc_lexical_utils_srcs})

###############################################################################
## Testing Source #############################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    file(GLOB hzcc_lexical_utils_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    target_sources(hzcc_test
            PRIVATE
            ${hzcc_lexical_utils_test}
            ${hzcc_lexical_utils_hdrs} ${hzcc_lexical_utils_srcs})
endif ()
