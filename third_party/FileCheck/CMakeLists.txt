###############################################################################
## File Checking Tool #########################################################
###############################################################################
# exec target
add_executable(HZCC_FileCheck)

set_target_properties(HZCC_FileCheck PROPERTIES FOLDER "Tests")

###############################################################################
## Compile Options ############################################################
###############################################################################
# compile versions (cpp17 inorder to use std::filesystem)
target_compile_features(HZCC_FileCheck PRIVATE cxx_std_17)

# hide static link symbols
if(APPLE)
#    target_link_options(HZCC_FileCheck PRIVATE -hidden-lssl)
else()
    target_link_options(HZCC_FileCheck PRIVATE -Wl,--exclude-libs,ALL)
endif()

# hide internal_status symbols
set_target_properties(HZCC_FileCheck PROPERTIES C_VISIBILITY_PRESET hidden) # not export symbols
set_target_properties(HZCC_FileCheck PROPERTIES CXX_VISIBILITY_PRESET hidden) # not export symbols

# always compile with optimization
target_compile_options(HZCC_FileCheck PRIVATE -fPIC -ffast-math -Bsymbolic -Werror)

# add current folder to include path
target_include_directories(HZCC_FileCheck PUBLIC .)

###############################################################################
## Dependence #################################################################
###############################################################################
# google effcee
FILE(GLOB EFFCEE_HDRS "effcee/*.h")
FILE(GLOB EFFCEE_SRCS "effcee/*.cc")
target_sources(HZCC_FileCheck PUBLIC ${EFFCEE_HDRS} ${EFFCEE_SRCS})

# google flag
find_package(gflags CONFIG REQUIRED)
target_link_libraries(HZCC_FileCheck gflags_static)
target_include_directories(HZCC_FileCheck PRIVATE ${GFLAGS_INCLUDE_DIR})

# google log
find_package(glog CONFIG REQUIRED)
target_link_libraries(HZCC_FileCheck glog::glog)
target_include_directories(HZCC_FileCheck PRIVATE ${glog_INCLUDE_DIRS})

# re2
find_package(re2 CONFIG REQUIRED)
target_link_libraries(HZCC_FileCheck re2::re2)

# threading
find_package(Threads REQUIRED)
target_link_libraries(HZCC_FileCheck Threads::Threads)

###############################################################################
## Sources ####################################################################
###############################################################################
target_sources(HZCC_FileCheck PUBLIC "FileCheck.cc")
