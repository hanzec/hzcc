cmake_minimum_required(VERSION 3.16.0) # we need FetchContent

###############################################################################
## CMake Settings #############################################################
###############################################################################
# build root checks
if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    if (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
        message(FATAL_ERROR "Building in source is not supported, consider build with -B/path/to/my/build/folder")
    endif ()
endif ()

# if not simplified build type, set default to Release
if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "CMAKE_BUILD_TYPE is not set, defaulting to ${CMAKE_BUILD_TYPE}")
endif ()

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
        GIT_TAG 2022.01.01
        GIT_REPOSITORY https://github.com/microsoft/vcpkg.git)
FetchContent_MakeAvailable(vcpkg)

# generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add custom cmake scrips location
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

###############################################################################
## Setup vcpkg ################################################################
###############################################################################
# run vcpkg bootstrap
if ((EXISTS "${vcpkg_SOURCE_DIR}/vcpkg") OR (EXISTS "${vcpkg_SOURCE_DIR}/vcpkg.exe"))
    message(VERBOSE "Found builded vcpkg at ${vcpkg_SOURCE_DIR}")
else ()
    message(STATUS "Not Found builded vcpkg, bootstrap vcpkg now...")
    if (WIN32)
        execute_process(COMMAND "${vcpkg_SOURCE_DIR}/bootstrap-vcpkg.bat" -disableMetrics
                RESULT_VARIABLE VCPKG_STATUS
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
        execute_process(COMMAND "${vcpkg_SOURCE_DIR}/vcpkg.exe" integrate install
                RESULT_VARIABLE VCPKG_STATUS
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    else ()
        execute_process(COMMAND bash "${vcpkg_SOURCE_DIR}/bootstrap-vcpkg.sh" -disableMetrics
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
        message(FATAL_ERROR " Not Found VCPKG cmake scripts at Source/third_party/scripts/buildsystems/vcpkg.cmake !")
    endif ()
else ()
    message(VERBOSE "CMAKE_TOOLCHAIN_FILE is already define as ${CMAKE_TOOLCHAIN_FILE}")
endif ()

###############################################################################
## Project Config #############################################################
###############################################################################
project(mycc VERSION 0.1.0)

# project lib export folder
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

###############################################################################
## Testing ####################################################################
###############################################################################
# initial test
enable_testing()
include(GoogleTest)

###############################################################################
## Packing ####################################################################
###############################################################################
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

###############################################################################
## Project Source #############################################################
###############################################################################
add_subdirectory(Source)

###############################################################################
## Documentation Source #######################################################
###############################################################################
add_subdirectory(Documentation)
