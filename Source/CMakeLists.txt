###############################################################################
## Project Versions ###########################################################
###############################################################################
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

if (ENABLE_TESTING)
    # google log
    target_link_libraries(mycc gflags_static)

    # google flag
    find_package(gflags CONFIG REQUIRED)
    target_link_libraries(mycc_test glog::glog)

    # google test deps(require pthread)
    find_package(Threads REQUIRED)
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(mycc_test Threads::Threads)
    target_link_libraries(mycc_test GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
endif ()

###############################################################################
## Compile Config #############################################################
###############################################################################
# build type specific options
if (CMAKE_BUILD_TYPE)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # general debug options
        target_link_options(mycc PRIVATE -o0 -g)
        target_compile_options(mycc PRIVATE -O0 -g)

        if (ENABLE_TESTING MATCHES "ON")
            target_link_options(mycc_test PRIVATE -o0 -g)
            target_compile_options(mycc_test PRIVATE -O0 -g)
        endif ()

        # Sentilizers
        if (ENABLE_ASAN MATCHES "ON")
            message(STATUS "Enable ASAN")

            target_link_options(mycc PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_compile_options(mycc PRIVATE -fsanitize=address -fno-omit-frame-pointer)

            if (ENABLE_TESTING MATCHES "ON")
                target_link_options(mycc_test PRIVATE -fsanitize=address -fno-omit-frame-pointer)
                target_compile_options(mycc_test PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            endif ()
        endif ()

        if (ENABLE_TSAN MATCHES "ON")
            message(STATUS "Enable TSAN")
            target_link_options(mycc PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
            target_compile_options(mycc PRIVATE -fsanitize=thread -fno-omit-frame-pointer)

            if (ENABLE_TESTING MATCHES "ON")
                target_link_options(mycc_test PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
                target_compile_options(mycc_test PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
            endif ()
        endif ()

        if (ENABLE_UBSAN MATCHES "ON")
            message(STATUS "Enable UBSAN")
            target_link_options(mycc PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)
            target_compile_options(mycc PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)

            if (ENABLE_TESTING MATCHES "ON")
                target_link_options(mycc_test PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)
                target_compile_options(mycc_test PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)
            endif ()
        endif ()

        if (ENABLE_MSAN MATCHES "ON")
            message(STATUS "Enable MSAN")
            target_link_options(mycc PRIVATE -fsanitize=memory -fno-omit-frame-pointer)
            target_compile_options(mycc PRIVATE -fsanitize=memory -fno-omit-frame-pointer)

            if (ENABLE_TESTING MATCHES "ON")
                target_link_options(mycc_test PRIVATE -fsanitize=memory -fno-omit-frame-pointer)
                target_compile_options(mycc_test PRIVATE -fsanitize=memory -fno-omit-frame-pointer)
            endif ()
        endif ()
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(mycc PRIVATE -O3 -fPIC -ffast-math -Bsymbolic -Werror)

        if (ENABLE_TESTING MATCHES "ON")
            target_compile_options(mycc_test PRIVATE -O3 -fPIC -ffast-math -Bsymbolic -Werror)
        endif ()
    endif ()
endif ()

# general  compile options
add_compile_options(-Wall -Wextra -frtti)

# hide static link symbols
if (APPLE)
    #    target_link_options(mycc PRIVATE -hidden-lssl)
else ()
    #    target_link_options(mycc PRIVATE -Wl,--exclude-libs,ALL)
endif ()

# compile under cxx 17
target_compile_features(mycc PRIVATE cxx_std_17)
set_property(TARGET mycc PROPERTY CXX_STANDARD_REQUIRED ON)

# set default visibility
set_target_properties(mycc PROPERTIES POSITION_INDEPENDENT_CODE 1) # generate fpic
set_target_properties(mycc PROPERTIES VISIBILITY_INLINES_HIDDEN 1) # generate fpic
set_target_properties(mycc PROPERTIES C_VISIBILITY_PRESET hidden) # not export symbols
set_target_properties(mycc PROPERTIES CXX_VISIBILITY_PRESET hidden) # not export symbols

# change include base to project root
target_include_directories(mycc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# testing target options
if (ENABLE_TESTING MATCHES "ON")
    # compile under cxx 17
    target_compile_features(mycc_test PRIVATE cxx_std_17)
    set_property(TARGET mycc_test PROPERTY CXX_STANDARD_REQUIRED ON)

    # change include base to project root
    target_include_directories(mycc_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

###############################################################################
## Project Source #############################################################
###############################################################################
target_sources(mycc
        PRIVATE
        "const_msg.h" "macro.h" "main.cc" "options.cc" "CompileContext.cc" "CompileContext.h")

###############################################################################
## Code Generation ############################################################
###############################################################################
add_subdirectory(codegen)

###############################################################################
## Optimization  ##############################################################
###############################################################################
add_subdirectory(optimization)

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
    target_sources(mycc_test
            PRIVATE
            "test_main.cc" "options.cc")
endif ()


###############################################################################
## Testing  Coverage ##########################################################
###############################################################################
if ((ENABLE_COVERAGE MATCHES "ON") AND (ENABLE_TESTING MATCHES "ON"))
    include(code-coverage)
    target_code_coverage(mycc_test)
    message(STATUS "Code coverage enabled")
endif ()
