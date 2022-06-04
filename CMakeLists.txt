cmake_minimum_required(VERSION 3.20.0) # we need unzip files

###############################################################################
## CMake Settings #############################################################
###############################################################################
# build root checks
if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    if (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
        message(FATAL_ERROR "Building in source is not supported,
                             consider build with -B/path/to/my/build/folder")
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
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)
option(ENABLE_MSAN "Enable Memory Sanitizer" OFF)
option(ENABLE_COVERAGE "Enable Code Coverage" OFF)
option(ENABLE_TESTING "Enable unit Tests" OFF)
option(ENABLE_E2E_TESTING "Enable End to END Tests" OFF)
option(ENABLE_DOCUMENTATION "Enable Documentation Build" ON)

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

###############################################################################
## Prepare Third-party Dependency #############################################
###############################################################################
# Fetch VCPKG and make it available
message(STATUS "Fetching VCPKG...")
include(FetchContent)
FetchContent_Declare(
        vcpkg
        GIT_TAG master
        GIT_REPOSITORY https://github.com/microsoft/vcpkg.git)
FetchContent_MakeAvailable(vcpkg)

# setup vcpkg
list(APPEND CMAKE_MODULE_PATH "${vcpkg_SOURCE_DIR}/scripts/cmake")

# setup include directories
include_directories("${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include/")

###############################################################################
## Setup vcpkg ################################################################
###############################################################################
# run vcpkg bootstrap
if ((EXISTS "${vcpkg_SOURCE_DIR}/vcpkg") OR (EXISTS "${vcpkg_SOURCE_DIR}/vcpkg.exe"))
    message(VERBOSE "Found builded vcpkg at ${vcpkg_SOURCE_DIR}")
else ()
    message(STATUS "Not Found builded vcpkg, bootstrap vcpkg now...")
    if (WIN32)
        execute_process(
                COMMAND "${vcpkg_SOURCE_DIR}/bootstrap-vcpkg.bat" -disableMetrics
                RESULT_VARIABLE VCPKG_STATUS
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
        execute_process(
                COMMAND "${vcpkg_SOURCE_DIR}/vcpkg.exe" integrate install
                RESULT_VARIABLE VCPKG_STATUS
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    else ()
        execute_process(
                COMMAND bash "${vcpkg_SOURCE_DIR}/bootstrap-vcpkg.sh" -disableMetrics
                RESULT_VARIABLE VCPKG_STATUS
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif ()
    if (VCPKG_STATUS AND NOT VCPKG_STATUS EQUAL 0)
        message(FATAL_ERROR "VCPKG bootstrap failed, can't build VCPKG !")
    else ()
        message(STATUS "Building VCPKG success!")
    endif ()
endif ()

# set vcpkg to cmake toolchain file (has to before declaration of project)
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if (EXISTS "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake")
        message(VERBOSE "Found cmake scripts at ${vcpkg_SOURCE_DIR}")
        set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE PATH "toolchain file")
    else ()
        message(FATAL_ERROR "Not Found VCPKG cmake scripts at
                             Source/third_party/scripts/buildsystems/vcpkg.cmake")
    endif ()
else ()
    message(VERBOSE "CMAKE_TOOLCHAIN_FILE is already define as ${CMAKE_TOOLCHAIN_FILE}")
endif ()

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
## Project Config #############################################################
###############################################################################
project(mycc VERSION 0.1.0)

###############################################################################
## Compiler Targets ###########################################################
###############################################################################
# hzcc target
add_executable(hzcc)

# testing target
if (ENABLE_TESTING MATCHES "ON")
    message(STATUS "Enable testing...")
    add_executable(hzcc_test)
endif ()

###############################################################################
## Project Source #############################################################
###############################################################################
add_subdirectory(Source)

###############################################################################
## Documentation Source #######################################################
###############################################################################
if (ENABLE_DOCUMENTATION MATCHES "ON")
    message(STATUS "Enable documentation")
    add_subdirectory(Documentation)
endif ()

###############################################################################
## Documentation Source #######################################################
###############################################################################
if (ENABLE_E2E_TESTING MATCHES "ON")
    message(STATUS "Enable E2E testing")
    add_subdirectory(Test)
endif ()

###############################################################################
## Testing ####################################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    include(GoogleTest)
    gtest_discover_tests(hzcc_test)
endif ()

###############################################################################
## Packing ####################################################################
###############################################################################
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
