###############################################################################
## Project Versions ###########################################################
###############################################################################
# generate the configuration file
configure_file(
        version.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.h)
target_include_directories(hzcc PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if (ENABLE_TESTING MATCHES "ON")
    target_include_directories(hzcc_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endif ()

###############################################################################
## Shared Dependency ##########################################################
###############################################################################
# google log
find_package(glog CONFIG REQUIRED)
target_link_libraries(hzcc glog::glog)

if (ENABLE_TESTING)
    # google log
    target_link_libraries(hzcc gflags_static)

    # google flag
    find_package(gflags CONFIG REQUIRED)
    target_link_libraries(hzcc_test glog::glog)

    # google test deps(require pthread)
    find_package(Threads REQUIRED)
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(hzcc_test Threads::Threads)
    target_link_libraries(hzcc_test GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
endif ()

###############################################################################
## Compile Config #############################################################
###############################################################################
# build type specific options
if (CMAKE_BUILD_TYPE)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # general debug options
        target_link_options(hzcc PRIVATE -o0 -g)
        target_compile_options(hzcc PRIVATE -O0 -g)

        if (ENABLE_TESTING MATCHES "ON")
            target_link_options(hzcc_test PRIVATE -o0 -g)
            target_compile_options(hzcc_test PRIVATE -O0 -g)
        endif ()

        # Sentilizers
        if (ENABLE_ASAN MATCHES "ON")
            message(STATUS "Enable ASAN")

            target_link_options(hzcc PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_compile_options(hzcc PRIVATE -fsanitize=address -fno-omit-frame-pointer)

            if (ENABLE_TESTING MATCHES "ON")
                target_link_options(hzcc_test PRIVATE -fsanitize=address -fno-omit-frame-pointer)
                target_compile_options(hzcc_test PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            endif ()
        endif ()

        if (ENABLE_TSAN MATCHES "ON")
            message(STATUS "Enable TSAN")
            target_link_options(hzcc PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
            target_compile_options(hzcc PRIVATE -fsanitize=thread -fno-omit-frame-pointer)

            if (ENABLE_TESTING MATCHES "ON")
                target_link_options(hzcc_test PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
                target_compile_options(hzcc_test PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
            endif ()
        endif ()

        if (ENABLE_UBSAN MATCHES "ON")
            message(STATUS "Enable UBSAN")
            target_link_options(hzcc PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)
            target_compile_options(hzcc PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)

            if (ENABLE_TESTING MATCHES "ON")
                target_link_options(hzcc_test PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)
                target_compile_options(hzcc_test PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)
            endif ()
        endif ()

        if (ENABLE_MSAN MATCHES "ON")
            message(STATUS "Enable MSAN")
            target_link_options(hzcc PRIVATE -fsanitize=memory -fno-omit-frame-pointer)
            target_compile_options(hzcc PRIVATE -fsanitize=memory -fno-omit-frame-pointer)

            if (ENABLE_TESTING MATCHES "ON")
                target_link_options(hzcc_test PRIVATE -fsanitize=memory -fno-omit-frame-pointer)
                target_compile_options(hzcc_test PRIVATE -fsanitize=memory -fno-omit-frame-pointer)
            endif ()
        endif ()
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(hzcc PRIVATE -O3 -fPIC -ffast-math -Bsymbolic -Werror)

        if (ENABLE_TESTING MATCHES "ON")
            target_compile_options(hzcc_test PRIVATE -O3 -fPIC -ffast-math -Bsymbolic -Werror)
        endif ()
    endif ()
endif ()

# general  compile options
add_compile_options(-Wall -Wextra -frtti)

# hide static link symbols
if (APPLE)
    #    target_link_options(hzcc PRIVATE -hidden-lssl)
else ()
    #    target_link_options(hzcc PRIVATE -Wl,--exclude-libs,ALL)
endif ()

# compile under cxx 17
target_compile_features(hzcc PRIVATE cxx_std_17)

# set default visibility
set_property(TARGET hzcc PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(hzcc PROPERTIES C_VISIBILITY_PRESET hidden) # not export symbols
set_target_properties(hzcc PROPERTIES POSITION_INDEPENDENT_CODE 1) # generate fpic
set_target_properties(hzcc PROPERTIES VISIBILITY_INLINES_HIDDEN 1) # generate fpic
set_target_properties(hzcc PROPERTIES CXX_VISIBILITY_PRESET hidden) # not export symbols

# change include base to project root
target_include_directories(hzcc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# testing target options
if (ENABLE_TESTING MATCHES "ON")
    # compile under cxx 17
    target_compile_features(hzcc_test PRIVATE cxx_std_17)
    set_property(TARGET hzcc_test PROPERTY CXX_STANDARD_REQUIRED ON)

    # change include base to project root
    target_include_directories(hzcc_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

###############################################################################
## Project Source #############################################################
###############################################################################
target_sources(hzcc
        PRIVATE
        "const_msg.h" "macro.h" "main.cc" "options.cc" "options.h")

###############################################################################
## Code Generation ############################################################
###############################################################################
#add_subdirectory(codegen)

###############################################################################
## Optimization  ##############################################################
###############################################################################
#add_subdirectory(optimization)

###############################################################################
## ASTs #######################################################################
###############################################################################
add_subdirectory(AST)

###############################################################################
## Project Utils ##############################################################
###############################################################################
add_subdirectory(utils)

###############################################################################
## Lexical Analysis ###########################################################
###############################################################################
add_subdirectory(lexical)

###############################################################################
## Syntax Analysis ###########################################################
###############################################################################
add_subdirectory(syntax)

###############################################################################
## Testing ####################################################################
###############################################################################
if (ENABLE_TESTING MATCHES "ON")
    # add testing main
    target_sources(hzcc_test
            PRIVATE
            "test_main.cc" "options.cc")
endif ()


###############################################################################
## Testing  Coverage ##########################################################
###############################################################################
if ((ENABLE_COVERAGE MATCHES "ON") AND (ENABLE_TESTING MATCHES "ON"))
    include(code-coverage)
    target_code_coverage(hzcc_test)
    message(STATUS "Code coverage enabled")
endif ()
