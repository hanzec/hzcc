###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_parser_common_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_parser_common_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_parser_common_hdrs} ${hzcc_parser_common_srcs})

###############################################################################
## Testing ####################################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_parser_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(hzcc_test
            PRIVATE
            ${hzcc_parser_test}
            ${hzcc_parser_common_hdrs} ${hzcc_parser_common_srcs})
endif ()
