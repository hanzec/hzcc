###############################################################################
## Project Versions ###########################################################
###############################################################################
# generate build time
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

# collect head commit hash
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
            OUTPUT_VARIABLE GIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# get hostname
cmake_host_system_information(RESULT MACHINE_HOST_NAME QUERY HOSTNAME)

# generate the configuration file
configure_file(
        version.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.h)
target_include_directories(mycc PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
## Shared Dependency ##########################################################
###############################################################################
# google log
find_package(glog CONFIG REQUIRED)
target_link_libraries(mycc glog::glog)
target_link_libraries(mycc_test glog::glog)
target_include_directories(mycc PRIVATE ${glog_INCLUDE_DIRS})

# google flag
find_package(gflags CONFIG REQUIRED)
target_link_libraries(mycc gflags_static)
target_include_directories(mycc PRIVATE ${GFLAGS_INCLUDE_DIR})

# google test deps(require pthread)
find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)
target_link_libraries(mycc_test Threads::Threads)
target_link_libraries(mycc_test GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

###############################################################################
## Compile Config #############################################################
###############################################################################
# build type specific options
if (CMAKE_BUILD_TYPE)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(mycc PRIVATE -o0 -g -fsanitize=address -fno-omit-frame-pointer)
        target_compile_options(mycc PRIVATE -O0 -g -fsanitize=address -fno-omit-frame-pointer)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(mycc PRIVATE -O3 -fPIC -ffast-math -Bsymbolic -Werror)
    endif ()
endif ()

# general  compile options
add_compile_options(-Wall -Wextra)

# hide static link symbols
if(MACOSX)
    target_link_options(mycc PRIVATE -hidden-lssl)
else()
    target_link_options(mycc PRIVATE -Wl,--exclude-libs,ALL)
endif()

# compile under cxx 17
target_compile_features(mycc PRIVATE cxx_std_17)
target_compile_features(mycc_test PRIVATE cxx_std_17)
#set_property(TARGET mycc PROPERTY CXX_STANDARD_REQUIRED ON)
#set_property(TARGET mycc_test PROPERTY CXX_STANDARD_REQUIRED ON)

# set default visibility
set_target_properties(mycc PROPERTIES POSITION_INDEPENDENT_CODE 1) # generate fpic
set_target_properties(mycc PROPERTIES VISIBILITY_INLINES_HIDDEN 1) # generate fpic
set_target_properties(mycc PROPERTIES C_VISIBILITY_PRESET hidden) # not export symbols
set_target_properties(mycc PROPERTIES CXX_VISIBILITY_PRESET hidden) # not export symbols

# change include base to project root
target_include_directories(mycc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(mycc_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

###############################################################################
## Project Source #############################################################
###############################################################################
target_sources(mycc
        PRIVATE
        "const_msg.h" "macro.h" "main.cc")

###############################################################################
## Project Utils ##############################################################
###############################################################################
add_subdirectory(utils)

###############################################################################
## Lexical Analysis ###########################################################
###############################################################################
add_subdirectory(lexical)

###############################################################################
## Testing ####################################################################
###############################################################################
# add testing main
target_sources(mycc_test PRIVATE "test_main.cc")
