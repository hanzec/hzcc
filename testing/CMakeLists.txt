###############################################################################
## Mycc E2E Testing ###########################################################
###############################################################################
# a fake target which allow e2e test target require binary target
add_custom_target(mycc_e2e COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS mycc)

###############################################################################
## Testing Dependence #########################################################
###############################################################################
find_package (Python3 COMPONENTS Interpreter)

# check dependency of LLVM lit
if(Python3_FOUND)
    message(STATUS "Python3 found at ${Python3_EXECUTABLE}")
    # looking for required python packages
    execute_process(COMMAND Python3::Interpreter -c "import lit" RESULT_VARIABLE EXIT_CODE OUTPUT_QUIET)
    if(EXIT_CODE)
        message(FATAL_ERROR "Cannot find lit python package, please install it")
    endif()
endif()

###############################################################################
## Executing E2E Test #########################################################
###############################################################################
