###############################################################################
## JVM Instructions ###########################################################
###############################################################################
add_subdirectory(jvm_instr)

###############################################################################
## JVM Libc Implement #########################################################
###############################################################################
add_subdirectory(libc-java)

###############################################################################
## Codegen Units ##############################################################
###############################################################################
add_subdirectory(visitor_impl)

###############################################################################
## JVM Sources ################################################################
###############################################################################
file(GLOB hzcc_code_gen_jvm_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_code_gen_jvm_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_code_gen_jvm_hdrs} ${hzcc_code_gen_jvm_srcs})

###############################################################################
## JVM Bytecode Utils #########################################################
###############################################################################
file(GLOB hzcc_code_gen_jvm_utils_hdrs ${CMAKE_CURRENT_LIST_DIR}/utils/*.h)
file(GLOB hzcc_code_gen_jvm_utils_srcs ${CMAKE_CURRENT_LIST_DIR}/utils/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_code_gen_jvm_utils_hdrs} ${hzcc_code_gen_jvm_utils_srcs})

###############################################################################
## Testing hzcc #############################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_code_gen_jvm_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(hzcc_test
            PRIVATE
            ${hzcc_code_gen_jvm_test}
            ${hzcc_code_gen_jvm_hdrs} ${hzcc_code_gen_jvm_srcs}
            ${hzcc_code_gen_jvm_utils_hdrs} ${hzcc_code_gen_jvm_utils_srcs})
endif ()
