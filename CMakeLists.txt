cmake_minimum_required(VERSION 3.20.0) # we need unzip files
###############################################################################
## CMake Settings #############################################################
###############################################################################
# build root checks
if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    if (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
        message(FATAL_ERROR "Building in source is not supported,
                             consider build with -B /path/to/my/build/folder")
    endif ()
endif ()

# if not simplified build type, set default to Release
if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "CMAKE_BUILD_TYPE is not set, defaulting to ${CMAKE_BUILD_TYPE}")
endif ()

# generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add custom cmake scrips location
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# set generate path
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# target build options
option(ENABLE_ANTLR "Enable ANTLR" OFF)
option(E2E_TESTING "Enable End to END Tests" OFF)
option(UNIT_TESTING "Enable unit Tests" ON)
option(RUNTIME_CHECK "Enable runtime checks" ON)
option(GENERATE_DOCUMENTATION "Enable Documentation Build" OFF)

###############################################################################
## Compile Config #############################################################
###############################################################################
# detect is MACOS or not
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif ()

# compile defs for multi-platform compiling
if (WIN32)
    add_compile_definitions(WIN32)
elseif (UNIX AND NOT MACOSX)
    add_compile_definitions(UNIX)
elseif (MACOSX)
    add_compile_definitions(MACOSX)
endif ()

# add enable runtime checks macro if needed
if (RUNTIME_CHECK MATCHES "ON")
    add_compile_definitions(HZCC_ENABLE_RUNTIME_CHECK)
endif ()

# set thrid-party base path
set(THIRD_PARTY_BASE_PATH ${CMAKE_SOURCE_DIR}/third_party)

###############################################################################
## Prepare VCPKG ##############################################################
###############################################################################
# Fetch VCPKG and make it available
message(STATUS "Fetching VCPKG...")
include(FetchContent)
FetchContent_Declare(
        vcpkg
        GIT_TAG 2022.11.14
        GIT_REPOSITORY https://github.com/microsoft/vcpkg.git)
FetchContent_MakeAvailable(vcpkg)

# Add VCPKG to CMake
set(CMAKE_TOOLCHAIN_FILE ${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake
        CACHE STRING "CMake toolchain file")

# setup include directories
include_directories("${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include/")

###############################################################################
## Project Config #############################################################
###############################################################################
project(hzcc VERSION 0.1.0)

###############################################################################
## Prepare LLVM ###############################################################
###############################################################################
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

###############################################################################
## Third-Party ###############################################################
###############################################################################
add_subdirectory(third_party)

###############################################################################
## System Information  ########################################################
###############################################################################
# generate build time
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

# collect head commit hash
find_package(Git QUIET)
if (GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
            OUTPUT_VARIABLE GIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif ()

# get hostname
cmake_host_system_information(RESULT MACHINE_HOST_NAME QUERY HOSTNAME)

###############################################################################
## Project hzcc #############################################################
###############################################################################
add_subdirectory(hzcc)

###############################################################################
## E2E Test ###################################################################
###############################################################################
if (E2E_TESTING MATCHES "ON")
    message(STATUS "Enable E2E testing")

    # set ctest build flags
    include(ProcessorCount)
    ProcessorCount(N)
    if(NOT N EQUAL 0)
        set(CTEST_BUILD_FLAGS -j${N})
        set(ctest_test_args ${ctest_test_args} PARALLEL_LEVEL ${N})
    endif()

    # add e2e test
    enable_testing()
    add_subdirectory(test)
endif ()

###############################################################################
## Documentation ##############################################################
###############################################################################
if (ENABLE_DOCUMENTATION MATCHES "ON")
    message(STATUS "Enable documentation")
    add_subdirectory(doc)
endif ()

###############################################################################
## Packing ####################################################################
###############################################################################
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
