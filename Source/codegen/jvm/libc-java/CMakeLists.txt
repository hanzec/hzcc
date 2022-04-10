###############################################################################
## JVM Libc Implement #########################################################
###############################################################################
find_package(Java REQUIRED)

## custom target for compiling libc-java
add_custom_target(libc-java
        DEPENDS "${CMAKE_CURRENT_LIST_DIR}/libc.java"
        BYPRODUCTS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libc.class"
        COMMAND ${Java_JAVAC_EXECUTABLE} "${CMAKE_CURRENT_LIST_DIR}/libc.java" -d "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        COMMENT "Building libc.java"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# make mycc target depend on libc-java
add_dependencies(mycc libc-java)