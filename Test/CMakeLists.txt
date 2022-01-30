###############################################################################
## Mycc E2E Testing ###########################################################
###############################################################################
# exec target
add_executable(FileCheck)

# a fake target which allow e2e test target require binary target
add_custom_target(mycc_e2e_test COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS mycc FileCheck)

###############################################################################
## Helper Function ############################################################
###############################################################################
function(add_lit_test name directory)
    # check existence of test directory
    if(NOT EXISTS ${directory})
        message(FATAL_ERROR "Directory ${directory} does not exist.")
    endif()

    # test root
    set(CURRENT_TEST_ROOT ${CMAKE_CURRENT_BINARY_DIR}/${name})

    # copy all test files to test directory
    if(NOT EXISTS ${CURRENT_TEST_ROOT})
        file(MAKE_DIRECTORY ${CURRENT_TEST_ROOT})
    else()
        file(GLOB CURRENT_TEST_FILES "${directory}/*")
        foreach(file ${CURRENT_TEST_FILES})
            message(STATUS "Copying ${file} to ${CURRENT_TEST_ROOT}")
            configure_file(${file} "${CURRENT_TEST_ROOT}/" COPYONLY)
        endforeach()
    endif()

    # generate the environment variable
    configure_file(lit.site.cfg.py.in "${CURRENT_TEST_ROOT}/lit.site.cfg.py")

    # add test target
    add_test(NAME ${name}
             WORKING_DIRECTORY ${CURRENT_TEST_ROOT}
             COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm_lit.py ${CURRENT_TEST_ROOT})
endfunction()

###############################################################################
## FileCheckTool ##############################################################
###############################################################################
add_subdirectory(utils/FileCheck)

###############################################################################
## Testing Dependence #########################################################
###############################################################################
find_package(Python3 COMPONENTS Interpreter)

# check dependency of LLVM lit
if (Python3_FOUND)
    message(STATUS "Python3 found at ${Python3_EXECUTABLE}")
    # looking for required python packages
    execute_process(COMMAND Python3::Interpreter -c "import lit" RESULT_VARIABLE EXIT_CODE OUTPUT_QUIET)
    if (EXIT_CODE AND NOT EXIT_CODE STREQUAL 0)
        set(E2E_DEPENDS_MET true)
        set(LLVM_LIT_COMMAND "${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm_lit.py")
    else ()
        message(WARNING "Cannot find lit python package, will skip lit testing.")
    endif ()
endif ()

###############################################################################
## Executing E2E Test #########################################################
###############################################################################
# enable test target
enable_testing()

if (${E2E_DEPENDS_MET})
    message(STATUS "E2E testing is enabled.")
    message(VERBOSE "Running E2E test with ${LLVM_LIT_COMMAND}")

    # run e2e test
    add_lit_test(part0 ${CMAKE_CURRENT_SOURCE_DIR}/part0)
endif ()