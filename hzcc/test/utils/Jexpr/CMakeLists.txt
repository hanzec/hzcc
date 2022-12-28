###############################################################################
## Java assembler Tool #########################################################
###############################################################################

include_directories(${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
## Dependence #################################################################
###############################################################################
#flex
find_package(FLEX REQUIRED)
FLEX_TARGET(MyScanner tokens.l ${CMAKE_CURRENT_BINARY_DIR}/tokens.cc COMPILE_FLAGS --header-file=tokens.h)

###############################################################################
## Sources ####################################################################
###############################################################################
FILE(GLOB JEXPR_HDRS "*.h")
FILE(GLOB JEXPR_SRCS "*.cc")
add_executable(jexpr
        ${FLEX_MyScanner_OUTPUTS}
        ${JEXPR_HDRS} ${JEXPR_SRCS})

target_link_libraries(jexpr ${FLEX_LIBRARIES})

###############################################################################
## Compile Options ############################################################
###############################################################################
target_compile_features(FileCheck PRIVATE cxx_std_11)

# hide static link symbols
if (APPLE)
    #    target_link_options(FileCheck PRIVATE -hidden-lssl)
else ()
    target_link_options(jexpr PRIVATE -Wl,--exclude-libs,ALL)
endif ()

# hide internal symbols
set_target_properties(jexpr PROPERTIES C_VISIBILITY_PRESET hidden) # not export symbols
set_target_properties(jexpr PROPERTIES CXX_VISIBILITY_PRESET hidden) # not export symbols

# always compile with optimization
target_compile_options(jexpr PRIVATE -fPIC -ffast-math -Bsymbolic -Werror)

