###############################################################################
## JVM Bytecode Backend #######################################################
###############################################################################
add_subdirectory(jvm)

###############################################################################
## Codegen Sources ############################################################
###############################################################################
file(GLOB hzcc_code_gen_srcs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_code_gen_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_code_gen_srcs} ${hzcc_code_gen_hdrs})

###############################################################################
## Codegen Sources ############################################################
###############################################################################
file(GLOB hzcc_code_gen_utils_hdrs ${CMAKE_CURRENT_LIST_DIR}/utils/*.h)
file(GLOB hzcc_code_gen_utils_srcs ${CMAKE_CURRENT_LIST_DIR}/utils/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_code_gen_utils_hdrs} ${hzcc_code_gen_utils_srcs})

###############################################################################
## Testing Source #############################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_code_gen_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(hzcc_test
            PRIVATE
            ${hzcc_code_gen_test}
            ${hzcc_code_gen_srcs} ${hzcc_code_gen_hdrs})
endif ()
