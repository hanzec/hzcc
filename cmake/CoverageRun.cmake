# Build-time driver. Generated configuration separates paths/arguments from code;
# every subprocess is invoked directly, never through a shell.
cmake_minimum_required(VERSION 3.25)
include("${coverage_config}")
# LLVM records may resolve symlinks (notably /var -> /private/var on macOS).
# GCC/gcovr instead compare the original path spellings stored in the objects.
if(coverage_backend STREQUAL "LLVM")
    get_filename_component(coverage_source_root "${coverage_source_root}" REALPATH)
    get_filename_component(coverage_binary_root "${coverage_binary_root}" REALPATH)
endif()
set(work "${coverage_report_root}/${coverage_target}")
set(xml "${coverage_report_root}/${coverage_target}.xml")
file(MAKE_DIRECTORY "${work}")
file(LOCK "${work}/coverage.lock" GUARD PROCESS TIMEOUT 300)
set(dependency_pattern "(^|/)(_deps|vcpkg_installed|third_party)(/|$)")

# Escape literal path characters when supplying a path as a gcovr regex.
function(coverage_regex_path output path)
    string(REGEX REPLACE "([][+.*()^$?|\\\\])" "\\\\\\1" escaped "${path}")
    set(${output} "${escaped}" PARENT_SCOPE)
endfunction()

if(coverage_backend STREQUAL "GCC")
    coverage_regex_path(binary_pattern "${coverage_binary_root}")
    set(gcovr "${coverage_python}" -m gcovr --root "${coverage_source_root}"
        --exclude "${dependency_pattern}" --exclude "${binary_pattern}/")
    foreach(pattern IN LISTS coverage_excludes)
        list(APPEND gcovr --exclude "${pattern}")
    endforeach()
endif()

if(coverage_action STREQUAL "run")
    # A failed test or capture must not leave an old successful report behind.
    file(REMOVE "${xml}" "${work}/coverage.json" "${work}/coverage.lcov" "${work}/coverage.profdata")
    if(coverage_backend STREQUAL "LLVM")
        file(REMOVE_RECURSE "${work}/raw")
        file(MAKE_DIRECTORY "${work}/raw")
        execute_process(COMMAND "${CMAKE_COMMAND}" -E env
            "LLVM_PROFILE_FILE=${work}/raw/%p-%m.profraw"
            "${coverage_executable}" ${coverage_args}
            WORKING_DIRECTORY "${coverage_run_directory}" COMMAND_ERROR_IS_FATAL ANY)
        file(GLOB profiles "${work}/raw/*.profraw")
        if(NOT profiles)
            message(FATAL_ERROR "The test produced no LLVM coverage profiles")
        endif()
        execute_process(COMMAND "${coverage_llvm_profdata}" merge -sparse ${profiles}
            -o "${work}/coverage.profdata" COMMAND_ERROR_IS_FATAL ANY)
        execute_process(COMMAND "${coverage_llvm_cov}" export "${coverage_executable}"
            "-instr-profile=${work}/coverage.profdata" -format=lcov
            OUTPUT_FILE "${work}/all.lcov" COMMAND_ERROR_IS_FATAL ANY)
        include("${CMAKE_CURRENT_LIST_DIR}/CoverageFilter.cmake")
    else()
        # GCC writes into shared object directories. Serialize reset/run/capture,
        # then save a target/configuration snapshot for independent later reports.
        file(LOCK "${coverage_binary_root}/ccov/gcov.lock" GUARD PROCESS TIMEOUT 300)
        set(scan_excludes "${dependency_pattern}" "${binary_pattern}/ccov/")
        set(other_configs "${coverage_configurations}")
        list(REMOVE_ITEM other_configs "${coverage_config_name}")
        if(other_configs)
            list(JOIN other_configs "|" other_configs_pattern)
            list(APPEND scan_excludes "/(${other_configs_pattern})/")
        endif()
        file(GLOB_RECURSE counters "${coverage_binary_root}/*.gcda")
        foreach(counter IN LISTS counters)
            set(skip FALSE)
            foreach(pattern IN LISTS scan_excludes)
                if(counter MATCHES "${pattern}")
                    set(skip TRUE)
                endif()
            endforeach()
            if(NOT skip)
                file(REMOVE "${counter}")
            endif()
        endforeach()
        execute_process(COMMAND "${coverage_executable}" ${coverage_args}
            WORKING_DIRECTORY "${coverage_run_directory}" COMMAND_ERROR_IS_FATAL ANY)
        foreach(pattern IN LISTS scan_excludes)
            list(APPEND gcovr --gcov-exclude-directory "${pattern}")
        endforeach()
        # gcovr parses this option as a command; quote the executable internally
        # as well as passing it as one argument, for compiler paths with spaces.
        execute_process(COMMAND ${gcovr} --gcov-executable "\"${coverage_gcov}\""
            --json "${work}/coverage.json" "${coverage_binary_root}"
            WORKING_DIRECTORY "${coverage_source_root}" COMMAND_ERROR_IS_FATAL ANY)
        file(READ "${work}/coverage.json" snapshot)
        string(JSON source_count LENGTH "${snapshot}" files)
        if(source_count EQUAL 0)
            message(FATAL_ERROR "No project-owned source coverage remains after filtering")
        endif()
    endif()
elseif(coverage_action STREQUAL "report" OR coverage_action STREQUAL "export")
    if(coverage_backend STREQUAL "LLVM")
        if(NOT EXISTS "${work}/coverage.lcov")
            message(FATAL_ERROR "Missing LLVM capture; build ccov-${coverage_target} first")
        endif()
        if(coverage_action STREQUAL "export")
            execute_process(COMMAND "${coverage_python}" -m lcov_cobertura
                "${work}/coverage.lcov" --base-dir "${coverage_source_root}" --output "${xml}"
                COMMAND_ERROR_IS_FATAL ANY)
        else()
            file(STRINGS "${work}/coverage.lcov" sources REGEX "^SF:")
            list(TRANSFORM sources REPLACE "^SF:" "")
            execute_process(COMMAND "${coverage_llvm_cov}" report "${coverage_executable}"
                "-instr-profile=${work}/coverage.profdata" ${sources}
                COMMAND_ERROR_IS_FATAL ANY)
        endif()
    else()
        if(coverage_action STREQUAL "export")
            list(APPEND gcovr --xml "${xml}")
        endif()
        execute_process(COMMAND ${gcovr} --add-tracefile "${work}/coverage.json"
            WORKING_DIRECTORY "${coverage_source_root}" COMMAND_ERROR_IS_FATAL ANY)
    endif()
else()
    message(FATAL_ERROR "Unknown coverage action: ${coverage_action}")
endif()
