# This repository's FileCheck wrapper embeds effcee and is used by the lit suites.
set(GFLAGS_USE_TARGET_NAMESPACE ON)
find_package(gflags CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(Threads REQUIRED)

file(GLOB EFFCEE_SRCS CONFIGURE_DEPENDS "effcee/*.cc")
# Modern RE2 aliases StringPiece to std::string_view. Its old out-of-line
# implementation is neither needed nor compatible with that standard-library type.
list(REMOVE_ITEM EFFCEE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/effcee/stringpiece.cc")
add_executable(HZCC_FileCheck FileCheck.cc ${EFFCEE_SRCS})
target_compile_features(HZCC_FileCheck PRIVATE cxx_std_17)
set_target_properties(HZCC_FileCheck PROPERTIES
    FOLDER Tests
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON)
target_include_directories(HZCC_FileCheck PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(HZCC_FileCheck PRIVATE
    gflags::gflags glog::glog re2::re2 Threads::Threads)
