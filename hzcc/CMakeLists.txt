###############################################################################
## Compiler Targets ###########################################################
###############################################################################
# exec target
add_executable(hzcc)

# required cpp 17
set_property(TARGET hzcc PROPERTY CXX_STANDARD 17)
set_property(TARGET hzcc PROPERTY CXX_STANDARD_REQUIRED ON)

# testing target
if (UNIT_TESTING MATCHES "ON")
    message(STATUS "Enable testing...")
    add_executable(hzcc_test parser/common/test/keyword_test.cc)

    # required cpp 17 since hzcc require cpp 17
    set_property(TARGET hzcc_test PROPERTY CXX_STANDARD 17)
    set_property(TARGET hzcc_test PROPERTY CXX_STANDARD_REQUIRED ON)
endif ()

###############################################################################
## Project Versions ###########################################################
###############################################################################
# generate the configuration file
configure_file(
        version.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.h)
target_include_directories(hzcc PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if (UNIT_TESTING MATCHES "ON")
    target_include_directories(hzcc_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endif ()

###############################################################################
## Shared Dependency ##########################################################
###############################################################################
# google re2
find_package(re2 CONFIG REQUIRED)
target_link_libraries(hzcc re2::re2)

# google log
find_package(glog CONFIG REQUIRED)
target_link_libraries(hzcc glog::glog)

# magic enum
find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(hzcc magic_enum::magic_enum)

# google abseil
find_package(absl CONFIG REQUIRED)
target_link_libraries(hzcc absl::cord absl::inlined_vector) # for status/statusor
target_link_libraries(hzcc absl::flat_hash_set absl::node_hash_set) # for heterogeneous lookup

if (UNIT_TESTING)
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
set_target_properties(hzcc PROPERTIES VISIBILITY_INLINES_HIDDEN 1) # generate fpic
set_target_properties(hzcc PROPERTIES CXX_VISIBILITY_PRESET hidden) # not export symbols

# change include base to project root
target_include_directories(hzcc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# testing target options
if (UNIT_TESTING MATCHES "ON")
    # compile under cxx 17
    target_compile_features(hzcc_test PRIVATE cxx_std_17)
    set_property(TARGET hzcc_test PROPERTY CXX_STANDARD_REQUIRED ON)

    # change include base to project root
    target_include_directories(hzcc_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

# build type specific options
if (CMAKE_BUILD_TYPE)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # general debug options
        target_link_options(hzcc PRIVATE -o0 -g)
        target_compile_options(hzcc PRIVATE -O0 -g)

        # debug mode will always enable ASAN
        target_link_options(hzcc PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_compile_options(hzcc PRIVATE -fsanitize=address -fno-omit-frame-pointer)

        if (UNIT_TESTING MATCHES "ON")
            target_link_options(hzcc_test PRIVATE -o0 -g)
            target_compile_options(hzcc_test PRIVATE -O0 -g)

            # debug mode will always enable ASAN
            target_link_options(hzcc_test PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_compile_options(hzcc_test PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        endif ()
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(hzcc PROPERTIES POSITION_INDEPENDENT_CODE 1)  # generate fpic
        target_compile_options(hzcc PRIVATE -O3 -ffast-math -Bsymbolic -Werror)

        if (UNIT_TESTING MATCHES "ON")
            set_target_properties(hzcc_test PROPERTIES POSITION_INDEPENDENT_CODE 1) # generate fpic

            target_compile_options(hzcc_test PRIVATE -O3 -fPIC -ffast-math -Bsymbolic -Werror)
        endif ()
    endif ()
endif ()

###############################################################################
## Project hzcc #############################################################
###############################################################################
target_sources(hzcc
        PRIVATE
        "macro.h" "main.cc" "options.cc" "options.h" "const_msg.h" "enums.h")

###############################################################################
## StatusCode Generation ############################################################
###############################################################################
#add_subdirectory(codegen)

###############################################################################
## Optimization  ##############################################################
###############################################################################
#add_subdirectory(optimization)

###############################################################################
## ASTs #######################################################################
###############################################################################
add_subdirectory(ast)

###############################################################################
## Project Utils ##############################################################
###############################################################################
add_subdirectory(utils)

###############################################################################
## lexical Analysis ###########################################################
###############################################################################
add_subdirectory(parser)

###############################################################################
## Testing ####################################################################
###############################################################################
if (UNIT_TESTING MATCHES "ON")
    # discover test
    include(GoogleTest)
    gtest_discover_tests(hzcc_test)

    # add testing main
    target_sources(hzcc_test
            PRIVATE
            "test_main.cc" "options.cc")
endif ()

###############################################################################
## Testing  Coverage ##########################################################
###############################################################################
if ((ENABLE_COVERAGE MATCHES "ON") AND (UNIT_TESTING MATCHES "ON"))
    include(code-coverage)
    target_code_coverage(hzcc_test)
    message(STATUS "Code coverage enabled")
endif ()
