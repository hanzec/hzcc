###############################################################################
## syntax parsers ##############################################################
###############################################################################
add_subdirectory(parser)

###############################################################################
## Syntax Sources ############################################################
###############################################################################
file(GLOB hzcc_syntax_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_syntax_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_syntax_hdrs} ${hzcc_syntax_srcs})

###############################################################################
## Syntax Utils ###############################################################
###############################################################################
file(GLOB hzcc_syntax_hdrs ${CMAKE_CURRENT_LIST_DIR}/utils/*.h)
file(GLOB hzcc_syntax_srcs ${CMAKE_CURRENT_LIST_DIR}/utils/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_syntax_hdrs} ${hzcc_syntax_srcs})

###############################################################################
## Testing Source #############################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_syntax_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(hzcc_test PRIVATE
            ${hzcc_syntax_test}
            ${hzcc_syntax_hdrs} ${hzcc_syntax_srcs}
            ${hzcc_syntax_hdrs} ${hzcc_syntax_srcs})
endif ()
