###############################################################################
## hzcc E2E Testing ###########################################################
###############################################################################
ProcessorCount(N)
add_custom_target(hzcc_e2e_test
        DEPENDS hzcc HZCC_FileCheck
        COMMAND ${CMAKE_CTEST_COMMAND} --rerun-failed --output-on-failure)

# set-up xunit output folder
set(TEST_OUTPUT_FOLDER "${CMAKE_CURRENT_BINARY_DIR}/test_output")

# create xunit output folder if not exist
if (NOT EXISTS "${TEST_OUTPUT_FOLDER}")
    file(MAKE_DIRECTORY "${TEST_OUTPUT_FOLDER}")
endif ()

# llvm-lit executable
set(LLVM_LIT_EXECUTABLE "${THIRD_PARTY_BASE_PATH}/lit/lit.py")

###############################################################################
## Helper Function ############################################################
###############################################################################
function(add_lit_test_set name directory)
    message(STATUS "Add lit test set [${name}]")

    # check existence of test directory
    if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${directory}")
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
            message(VERBOSE "Discover test [${name}]:${file}")
            configure_file(${file} "${CURRENT_TEST_ROOT}/" COPYONLY)
        endif ()
    endforeach ()

    # generate the environment variable
    configure_file(lit.site.cfg.py.in "${CURRENT_TEST_ROOT}/lit.site.cfg.py")

    # add test targets
    set(TEST_TARGET_OUTPUT_FILE "${TEST_OUTPUT_FOLDER}/${name}.xunit.xml")
    add_test(NAME ${name}
            WORKING_DIRECTORY ${CURRENT_TEST_ROOT}
            COMMAND ${Python3_EXECUTABLE} ${LLVM_LIT_EXECUTABLE} --no-progress-bar --timeout 5 --xunit-xml-output "${TEST_TARGET_OUTPUT_FILE}" ${CURRENT_TEST_ROOT})
endfunction()

###############################################################################
## FileCheckTool ##############################################################
###############################################################################
# testing target required this target
add_dependencies(hzcc_e2e_test HZCC_FileCheck)

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
## Executing E2E test #########################################################
###############################################################################
# enable test target
enable_testing()

message(STATUS "E2E testing is enabled.")

# add compiler_info test set
add_lit_test_set(compiler_info compiler_info)

# add lexical test set
add_lit_test_set(lexical lexical)

# add test_set_0 test set
add_lit_test_set(lexical_part3 syntax/test_set_0)
