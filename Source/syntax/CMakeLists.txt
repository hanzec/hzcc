###############################################################################
## Project Sources ############################################################
###############################################################################
file(GLOB MYCC_SYNTAX_HDRS ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB MYCC_SYNTAX_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cc)

# project sources
target_sources(mycc PRIVATE ${MYCC_SYNTAX_HDRS} ${MYCC_SYNTAX_SRCS})

###############################################################################
## Syntax Utils ##############################################################
###############################################################################
#add_subdirectory(utils)

###############################################################################
## Testing Source #############################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    # looking for the source code
    file(GLOB MYCC_SYNTAX_TEST_SRCS ${CMAKE_CURRENT_LIST_DIR}/test/*_test.cc)

    # add to mycc_test target
    target_sources(mycc_test
            PRIVATE
            ${MYCC_SYNTAX_TEST_SRCS}
            ${MYCC_SYNTAX_HDRS} ${MYCC_SYNTAX_SRCS})
endif ()