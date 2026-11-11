cmake_minimum_required(VERSION 3.25...4.4)

# Keep generated files, dependency builds and compiler outputs out of the source tree.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not supported. Use cmake -S . -B build/default")
endif()

get_property(HZCC_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT HZCC_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build configuration" FORCE)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(UNIT_TESTING "Build GoogleTest unit tests" ON)
option(E2E_TESTING "Register lit end-to-end tests" OFF)
option(RUNTIME_CHECK "Enable compiler runtime assertions" ON)
option(ENABLE_COVERAGE "Instrument unit tests for code coverage" OFF)
option(GENERATE_DOCUMENTATION "Enable Doxygen and optional LaTeX documentation" OFF)

# vcpkg's toolchain and manifest features must be selected before project().
include(Dependencies)
project(hzcc VERSION 0.1.0 DESCRIPTION "Experimental C compiler" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(THIRD_PARTY_BASE_PATH "${PROJECT_SOURCE_DIR}/third_party")

# Apply ASan to every Debug target, including the bundled FileCheck executable.
# Matching vcpkg Debug flags are defined in cmake/triplets/.
if(UNIX AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options("$<$<CONFIG:Debug>:-fsanitize=address;-fno-omit-frame-pointer>")
    add_link_options("$<$<CONFIG:Debug>:-fsanitize=address>")
endif()

if(APPLE)
    add_compile_definitions(MACOSX)
elseif(UNIX)
    add_compile_definitions(UNIX)
elseif(WIN32)
    add_compile_definitions(WIN32)
endif()
if(RUNTIME_CHECK)
    add_compile_definitions(HZCC_ENABLE_RUNTIME_CHECK)
endif()

# Enable CTest at the root so unit tests are discoverable without E2E testing.
if(UNIT_TESTING OR E2E_TESTING)
    enable_testing()
endif()

string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
set(GIT_HASH "unknown")
find_package(Git QUIET)
if(Git_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" log -1 --pretty=format:%h
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        OUTPUT_VARIABLE HZCC_GIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
        RESULT_VARIABLE HZCC_GIT_RESULT)
    if(HZCC_GIT_RESULT EQUAL 0)
        set(GIT_HASH "${HZCC_GIT_HASH}")
    endif()
endif()
cmake_host_system_information(RESULT MACHINE_HOST_NAME QUERY HOSTNAME)

add_subdirectory(hzcc)
if(E2E_TESTING)
    add_subdirectory(third_party)
    add_subdirectory(test)
endif()
if(GENERATE_DOCUMENTATION)
    add_subdirectory(doc)
endif()

set(CPACK_PROJECT_NAME "${PROJECT_NAME}")
set(CPACK_PROJECT_VERSION "${PROJECT_VERSION}")
include(CPack)
