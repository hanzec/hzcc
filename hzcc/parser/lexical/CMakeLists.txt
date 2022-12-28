###############################################################################
## lexical Utils ##############################################################
###############################################################################
add_subdirectory(utils)

###############################################################################
## lexical Utils ##############################################################
###############################################################################
file(GLOB hzcc_lexical_static_hdrs ${CMAKE_CURRENT_LIST_DIR}/static/*.h)
file(GLOB hzcc_lexical_static_srcs ${CMAKE_CURRENT_LIST_DIR}/static/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_lexical_static_hdrs} ${hzcc_lexical_static_srcs})

###############################################################################
## lexical Sources ############################################################
###############################################################################
file(GLOB hzcc_lexical_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_lexical_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_lexical_hdrs} ${hzcc_lexical_srcs})

###############################################################################
## Testing hzcc #############################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_lexical_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(hzcc_test
            PRIVATE
            ${hzcc_lexical_test}
            ${hzcc_lexical_hdrs} ${hzcc_lexical_srcs}
            ${hzcc_lexical_static_hdrs} ${hzcc_lexical_static_srcs})
endif ()
