find_package(Python3 3.6 COMPONENTS Interpreter REQUIRED)
# Per-test timeouts require psutil to terminate the complete subprocess tree.
execute_process(
    COMMAND "${Python3_EXECUTABLE}" -c "import psutil"
    RESULT_VARIABLE psutil_result
    ERROR_QUIET)
if(NOT psutil_result EQUAL 0)
    message(FATAL_ERROR
        "E2E test timeouts require psutil. Install requirements-test.txt into your "
        "Python environment, then set Python3_EXECUTABLE to that interpreter.")
endif()

# lit.py imports the adjacent vendored package; no system pip installation is needed.
set(LLVM_LIT_EXECUTABLE "${THIRD_PARTY_BASE_PATH}/lit/lit.py")
execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${LLVM_LIT_EXECUTABLE}" --version
    RESULT_VARIABLE lit_result
    OUTPUT_QUIET
    ERROR_VARIABLE lit_error)
if(NOT lit_result EQUAL 0)
    message(FATAL_ERROR "Cannot run the bundled LLVM lit: ${lit_error}")
endif()

set(TEST_OUTPUT_FOLDER "${CMAKE_CURRENT_BINARY_DIR}/test_output")
file(MAKE_DIRECTORY "${TEST_OUTPUT_FOLDER}")

function(add_lit_test_set name directory)
    set(CURRENT_TEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/${directory}")
    set(test_exec_root "${CMAKE_CURRENT_BINARY_DIR}/${name}/$<CONFIG>")
    # Expand CMake variables first and target/configuration expressions at generation.
    configure_file(lit.site.cfg.py.in "${CMAKE_CURRENT_BINARY_DIR}/${name}.site.py.in" @ONLY)
    file(GENERATE
        OUTPUT "${test_exec_root}/lit.site.cfg.py"
        INPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.site.py.in")
    add_test(NAME ${name}
        COMMAND "${Python3_EXECUTABLE}" "${LLVM_LIT_EXECUTABLE}"
            --no-progress-bar --verbose --timeout 5
            --xunit-xml-output "${TEST_OUTPUT_FOLDER}/${name}-$<CONFIG>.xunit.xml"
            "${test_exec_root}")
    set_tests_properties(${name} PROPERTIES
        WORKING_DIRECTORY "${test_exec_root}"
        LABELS e2e)
endfunction()

add_lit_test_set(compiler_info compiler_info)
add_lit_test_set(lexical lexical)
add_lit_test_set(lexical_part3 syntax/test_set_0)

# Run every E2E suite, including on the first build when no failures are recorded.
add_custom_target(hzcc_e2e_test
    COMMAND "${CMAKE_CTEST_COMMAND}" -C "$<CONFIG>" --output-on-failure -L e2e
    DEPENDS hzcc HZCC_FileCheck
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    USES_TERMINAL
    VERBATIM)
