###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB MYCC_LEXICAL_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB MYCC_LEXICAL_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_LEXICAL_HDRS} ${MYCC_LEXICAL_SRCS})

###############################################################################
## Lexical Utils ##############################################################
###############################################################################
add_subdirectory(utils)

###############################################################################
## Testing Source #############################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    file(GLOB MYCC_LEXICAL_TEST_SRCS ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    target_sources(mycc_test
            PRIVATE
            ${MYCC_LEXICAL_TEST_SRCS}
            ${MYCC_LEXICAL_HDRS} ${MYCC_LEXICAL_SRCS})
endif ()