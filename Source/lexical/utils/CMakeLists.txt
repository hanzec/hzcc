###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB MYCC_LEXICAL_UTILS_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB MYCC_LEXICAL_UTILS_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_LEXICAL_UTILS_HDRS} ${MYCC_LEXICAL_UTILS_SRCS})

###############################################################################
## Testing Source #############################################################
###############################################################################
file(GLOB MYCC_LEXICAL_UTILS_TEST_SRCS ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

target_sources(mycc_test
        PRIVATE
        ${MYCC_LEXICAL_UTILS_TEST_SRCS}
        ${MYCC_LEXICAL_UTILS_HDRS} ${MYCC_LEXICAL_UTILS_SRCS})