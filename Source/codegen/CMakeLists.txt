###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB MYCC_CODE_GEN_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB MYCC_CODE_GEN_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_CODE_GEN_HDRS} ${MYCC_CODE_GEN_SRCS})

###############################################################################
## JVM Bytecode Backend #######################################################
###############################################################################
add_subdirectory(jvm)

###############################################################################
## Testing Source #############################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB MYCC_CODE_GEN_TEST_SRCS ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(mycc_test
            PRIVATE
            ${MYCC_CODE_GEN_TEST_SRCS}
            ${MYCC_CODE_GEN_HDRS} ${MYCC_CODE_GEN_SRCS})
endif ()
