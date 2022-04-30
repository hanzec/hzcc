###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB MYCC_CODE_GEN_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB MYCC_CODE_GEN_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_CODE_GEN_HDRS} ${MYCC_CODE_GEN_SRCS})

###############################################################################
## Codegen Units ##############################################################
###############################################################################
add_subdirectory(visitor_impl)

###############################################################################
## JVM Bytecode Utils #########################################################
###############################################################################
file(GLOB MYCC_CODE_GEN_UTILS_HDRS ${CMAKE_CURRENT_LIST_DIR}/utils/*.h)
file(GLOB MYCC_CODE_GEN_UTILS_SRCS ${CMAKE_CURRENT_LIST_DIR}/utils/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_CODE_GEN_UTILS_HDRS} ${MYCC_CODE_GEN_UTILS_SRCS})

###############################################################################
## JVM Libc Implement #########################################################
###############################################################################
add_subdirectory(libc-java)

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
            ${MYCC_CODE_GEN_HDRS} ${MYCC_CODE_GEN_SRCS}
            ${MYCC_CODE_GEN_UTILS_HDRS} ${MYCC_CODE_GEN_UTILS_SRCS})
endif ()
