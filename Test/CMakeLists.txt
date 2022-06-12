###############################################################################
## Hzcc E2E Testing ###########################################################
###############################################################################
# a fake target which allow e2e test target require binary target
add_custom_target(mycc_e2e_test COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS hzcc)

###############################################################################
## Test Variables #############################################################
###############################################################################
# set-up xunit output folder
set(TEST_OUTPUT_FOLDER "${CMAKE_CURRENT_BINARY_DIR}/test_output")

# create xunit output folder if not exist
if (NOT EXISTS "${TEST_OUTPUT_FOLDER}")
    file(MAKE_DIRECTORY "${TEST_OUTPUT_FOLDER}")
endif ()

# llvm-lit executable
set(LLVM_LIT_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/utils/llvm_lit.py")

###############################################################################
## Helper Function ############################################################
###############################################################################
function(add_lit_test_set name directory)
    message(STATUS "Add lit test set [${name}]")

    # check existence of test directory
    if (NOT EXISTS ${directory})
        message(FATAL_ERROR "Directory ${directory} does not exist.")
    endif ()

    # test root
    set(CURRENT_TEST_ROOT "${CMAKE_CURRENT_BINARY_DIR}/${name}")

    # copy all test files to test directory
    if (NOT EXISTS ${CURRENT_TEST_ROOT})
        file(MAKE_DIRECTORY ${CURRENT_TEST_ROOT})
    endif ()

    # copy all test files to test directory
    file(GLOB CURRENT_TEST_FILES "${directory}/*")
    foreach (file ${CURRENT_TEST_FILES})
        get_filename_component(FILE_EXTENSION ${file} LAST_EXT)
        if (FILE_EXTENSION STREQUAL ".disabled")
            message(STATUS "Ignore test case [${name}]:${file} in  since it is mark as disabled.")
        else ()
            message(STATUS "Discover test [${name}]:${file}")
            configure_file(${file} "${CURRENT_TEST_ROOT}/" COPYONLY)
        endif ()
    endforeach ()

    # generate the environment variable
    configure_file(lit.site.cfg.py.in "${CURRENT_TEST_ROOT}/lit.site.cfg.py")

    # add test targets
    set(TEST_TARGET_OUTPUT_FILE "${TEST_OUTPUT_FOLDER}/${name}.xunit.xml")
    add_test(NAME ${name}
            WORKING_DIRECTORY ${CURRENT_TEST_ROOT}
            COMMAND ${Python3_EXECUTABLE} ${LLVM_LIT_EXECUTABLE} --xunit-xml-output "${TEST_TARGET_OUTPUT_FILE}" ${CURRENT_TEST_ROOT})
endfunction()

###############################################################################
## FileCheckTool ##############################################################
###############################################################################
add_subdirectory(utils/FileCheck)

# testing target required this target
add_dependencies(mycc_e2e_test FileCheck)

###############################################################################
## FileCheckTool ##############################################################
###############################################################################
#add_subdirectory(utils/Jexpr)
#
## testing target required this target
#add_dependencies(mycc_e2e_test jexpr)
#

###############################################################################
## Testing Dependence #########################################################
###############################################################################
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# check dependency of LLVM lit
if (Python3_FOUND)
    message(STATUS "Python3 found at ${Python3_EXECUTABLE}")
    # looking for required python packages
    execute_process(COMMAND Python3::Interpreter -c "import asd" RESULT_VARIABLE exit_code)
    if (${exit_code} EQUAL 1)
        message(FATAL_ERROR "Cannot find llvm lit python package.")
    endif ()
else ()
    message(FATAL_ERROR "Cannot find python3 interpreter.")
endif ()

###############################################################################
## Executing E2E Test #########################################################
###############################################################################
# enable test target
enable_testing()

message(STATUS "E2E testing is enabled.")

# add compiler_info test set
add_lit_test_set(compiler_info "${CMAKE_CURRENT_LIST_DIR}/compiler_info")

# add lexical test set
add_lit_test_set(lexical "${CMAKE_CURRENT_LIST_DIR}/lexical/")

# add test_set_0 test set
add_lit_test_set(lexical_part3 "${CMAKE_CURRENT_LIST_DIR}/syntax/test_set_0")
