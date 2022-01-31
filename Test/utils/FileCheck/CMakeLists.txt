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

# hide static link symbols
if(MACOSX)
    target_link_options(FileCheck PRIVATE -hidden-lssl)
else()
    target_link_options(FileCheck PRIVATE -Wl,--exclude-libs,ALL)
endif()

# hide internal symbols
set_target_properties(FileCheck PROPERTIES C_VISIBILITY_PRESET hidden) # not export symbols
set_target_properties(FileCheck PROPERTIES CXX_VISIBILITY_PRESET hidden) # not export symbols

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
find_package(re2 CONFIG REQUIRED)
target_link_libraries(FileCheck re2::re2)

# threading
find_package(Threads REQUIRED)
target_link_libraries(FileCheck Threads::Threads)