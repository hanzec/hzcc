include_guard(GLOBAL)

# Debug dependencies must use the same ASan ABI as the project, particularly
# Abseil's hash containers. User-provided overlays retain precedence.
list(APPEND VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_LIST_DIR}/triplets")
list(REMOVE_DUPLICATES VCPKG_OVERLAY_TRIPLETS)

# Only install optional libraries when their corresponding targets are requested.
if(UNIT_TESTING)
    list(APPEND VCPKG_MANIFEST_FEATURES tests)
endif()
if(ENABLE_ANTLR)
    list(APPEND VCPKG_MANIFEST_FEATURES antlr)
endif()
list(REMOVE_DUPLICATES VCPKG_MANIFEST_FEATURES)

# An explicit toolchain takes precedence, e.g. an existing vcpkg installation.
if(NOT CMAKE_TOOLCHAIN_FILE AND NOT DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
    file(READ "${CMAKE_CURRENT_LIST_DIR}/../vcpkg.json" HZCC_VCPKG_MANIFEST)
    string(JSON HZCC_VCPKG_BASELINE GET "${HZCC_VCPKG_MANIFEST}" builtin-baseline)
    include(FetchContent)
    # Use the same immutable commit for the tool and the package registry.
    FetchContent_Declare(vcpkg
        GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
        GIT_TAG "${HZCC_VCPKG_BASELINE}"
        GIT_PROGRESS TRUE)
    FetchContent_MakeAvailable(vcpkg)
    set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake"
        CACHE FILEPATH "CMake toolchain file")
endif()
