###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB hzcc_code_gen_jvm_instr_hdrs ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB hzcc_code_gen_jvm_instr_srcs ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(hzcc PRIVATE ${hzcc_code_gen_jvm_instr_hdrs} ${hzcc_code_gen_jvm_instr_srcs})

###############################################################################
## Testing Source #############################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB hzcc_code_gen_jvm_instr_test ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(hzcc
            PRIVATE
            ${hzcc_code_gen_jvm_instr_test}
            ${hzcc_code_gen_jvm_instr_hdrs} ${hzcc_code_gen_jvm_instr_srcs})
endif ()
