###############################################################################
## File Checking Tool #########################################################
###############################################################################
target_sources(FileCheck PUBLIC "FileCheck.cc")

# add current folder to include path
target_include_directories(FileCheck PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

###############################################################################
## Compile Options ############################################################
###############################################################################
# compile versions (cpp17 inorder to use std::filesystem)
target_compile_features(FileCheck PRIVATE cxx_std_17)

###############################################################################
## Dependence #################################################################
###############################################################################
# google effcee
FILE(GLOB EFFCEE_HDRS "effcee/*.h")
FILE(GLOB EFFCEE_SRCS "effcee/*.cc")
target_sources(FileCheck PUBLIC ${EFFCEE_HDRS} ${EFFCEE_SRCS})

# google flag
find_package(gflags CONFIG REQUIRED)
target_link_libraries(FileCheck gflags_static)
target_include_directories(FileCheck PRIVATE ${GFLAGS_INCLUDE_DIR})

# re2
find_package(RE2 CONFIG REQUIRED)
target_link_libraries(FileCheck re2::re2)

# threading
find_package(Threads REQUIRED)
target_link_libraries(FileCheck Threads::Threads)