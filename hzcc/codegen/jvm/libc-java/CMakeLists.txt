find_package(Java REQUIRED COMPONENTS Development)

# Match the compiler's runtime directory for single- and multi-config generators.
set(hzcc_java_output_dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
get_property(hzcc_java_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(hzcc_java_multi_config)
    string(APPEND hzcc_java_output_dir "/$<CONFIG>")
endif()
set(hzcc_java_class "${hzcc_java_output_dir}/libc.class")

add_custom_command(
    OUTPUT "${hzcc_java_class}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${hzcc_java_output_dir}"
    COMMAND "${Java_JAVAC_EXECUTABLE}" -d "${hzcc_java_output_dir}"
            "${CMAKE_CURRENT_SOURCE_DIR}/libc.java"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/libc.java" "${Java_JAVAC_EXECUTABLE}"
    COMMENT "Compiling the JVM libc bridge"
    VERBATIM)
add_custom_target(libc-java DEPENDS "${hzcc_java_class}")

# Consumers of the backend objects share the same Java runtime prerequisite.
add_dependencies(hzcc_core libc-java)
