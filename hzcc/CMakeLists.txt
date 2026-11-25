# Keep dependency usage requirements identical for the compiler and unit tests.
find_package(absl CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
include(GlogDependencies)
find_package(magic_enum CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(Threads REQUIRED)

add_library(hzcc_dependencies INTERFACE)
target_link_libraries(hzcc_dependencies INTERFACE
    absl::cord
    absl::flat_hash_map
    absl::flat_hash_set
    absl::inlined_vector
    absl::node_hash_set
    absl::strings
    glog::glog
    magic_enum::magic_enum
    re2::re2
    Threads::Threads)

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/version.h")

function(hzcc_configure_target target visibility)
    target_compile_features(${target} ${visibility} cxx_std_17)
    set_target_properties(${target} PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        POSITION_INDEPENDENT_CODE ON)
    target_include_directories(${target} ${visibility}
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(${target} ${visibility} hzcc_dependencies)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -frtti)
    endif()
endfunction()

# Object libraries retain translation units whose only effect is factory registration.
# Compile production sources once and use the same objects in both executables.
add_library(hzcc_core OBJECT options.cc)
hzcc_configure_target(hzcc_core PUBLIC)

add_executable(hzcc main.cc)
hzcc_configure_target(hzcc PRIVATE)
target_link_libraries(hzcc PRIVATE hzcc_core CLI11::CLI11)

if(UNIT_TESTING)
    find_package(GTest CONFIG REQUIRED)
    # The project supplies test_main.cc, so do not link either GTest main library.
    add_executable(hzcc_test test_main.cc)
    hzcc_configure_target(hzcc_test PRIVATE)
    target_link_libraries(hzcc_test PRIVATE hzcc_core GTest::gmock GTest::gtest)
endif()

# Each directory owns its non-recursive source list. CONFIGURE_DEPENDS makes adding
# a source or test trigger reconfiguration without duplicating production sources.
function(hzcc_add_sources)
    if(ARGC EQUAL 0)
        set(source_dirs "${CMAKE_CURRENT_SOURCE_DIR}")
    else()
        set(source_dirs ${ARGV})
    endif()
    foreach(source_dir IN LISTS source_dirs)
        file(GLOB sources CONFIGURE_DEPENDS "${source_dir}/*.cc" "${source_dir}/*.h")
        target_sources(hzcc_core PRIVATE ${sources})
        if(UNIT_TESTING)
            file(GLOB tests CONFIGURE_DEPENDS "${source_dir}/test/*_test.cc")
            target_sources(hzcc_test PRIVATE ${tests})
        endif()
    endforeach()
endfunction()

# Subdirectories register their production sources and tests through the helper.
add_subdirectory(ast)
add_subdirectory(utils)
add_subdirectory(parser)

# JVM code generation and optimization are unfinished and are not built by default.
if(UNIT_TESTING)
    include(GoogleTest)
    gtest_discover_tests(hzcc_test
        DISCOVERY_MODE PRE_TEST
        PROPERTIES LABELS unit)
    if(ENABLE_COVERAGE)
        include(code-coverage)
        target_code_coverage(hzcc_core PUBLIC)
        target_code_coverage(hzcc_test)
    endif()
endif()
