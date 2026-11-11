# Legacy JVM inspection utility. The default HZCC build does not enable it.
find_package(FLEX REQUIRED)
flex_target(JexprScanner
    "${CMAKE_CURRENT_SOURCE_DIR}/tokens.l"
    "${CMAKE_CURRENT_BINARY_DIR}/tokens.cc"
    DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/tokens.h")

file(GLOB jexpr_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_executable(jexpr ${jexpr_sources} ${FLEX_JexprScanner_OUTPUTS})
target_include_directories(jexpr PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}")
# Preserve this vendored utility's original dialect; its global invoke type
# predates std::invoke and conflicts with that name under C++17.
target_compile_features(jexpr PRIVATE cxx_std_11)
set_target_properties(jexpr PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    POSITION_INDEPENDENT_CODE ON)
# tokens.l uses noyywrap, so the generated scanner needs no Flex runtime library.
if(MSVC)
    target_compile_options(jexpr PRIVATE /W4)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(jexpr PRIVATE -Wall -Wextra)
endif()
