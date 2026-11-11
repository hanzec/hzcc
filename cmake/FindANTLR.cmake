#[=======================================================================[.rst:
FindANTLR
---------
Find the ANTLR 4 generator JAR and a Java runtime. ANTLR_EXECUTABLE is the
JAR path, not a shell launcher. ANTLR_FOUND and ANTLR_VERSION describe a
generator that was successfully executed; REQUIRED, QUIET, and version
constraints follow find_package() semantics.

antlr_target(<name> <grammar> [LEXER] [PARSER] [LISTENER] [VISITOR]
             [PACKAGE <namespace>] [OUTPUT_DIRECTORY <directory>]
             [DEPENDS_ANTLR <name>] [DEPENDS <files>...]
             [COMPILE_FLAGS <options>...])

LEXER or PARSER alone denotes a separate grammar; neither or both denotes
a combined grammar. Listener and visitor output is opt-in. The function
exports ANTLR_<name>_INPUT, _OUTPUT_DIR, _CXX_OUTPUTS, and _OUTPUTS into the
caller scope. DEPENDS_ANTLR orders generation and supplies the dependent
grammar's output directory to ANTLR's token-vocabulary search path. Other
imported grammars must be listed explicitly in DEPENDS.
#]=======================================================================]

find_package(Java QUIET COMPONENTS Runtime)
find_file(ANTLR_EXECUTABLE
    NAMES "antlr-${ANTLR_FIND_VERSION}-complete.jar"
          antlr-4.13.2-complete.jar antlr4.jar antlr-4.jar antlr.jar
    PATH_SUFFIXES share/java lib)
mark_as_advanced(ANTLR_EXECUTABLE)

unset(ANTLR_VERSION)
set(_ANTLR_GENERATOR_WORKS FALSE)
set(_ANTLR_FAILURE "Set ANTLR_EXECUTABLE to an ANTLR 4 generator JAR.")
if(ANTLR_EXECUTABLE AND Java_JAVA_EXECUTABLE)
    execute_process(
        COMMAND "${Java_JAVA_EXECUTABLE}" -jar "${ANTLR_EXECUTABLE}"
        RESULT_VARIABLE _ANTLR_RESULT
        OUTPUT_VARIABLE _ANTLR_STDOUT
        ERROR_VARIABLE _ANTLR_STDERR
        TIMEOUT 30)
    if(_ANTLR_RESULT STREQUAL "0")
        string(REGEX MATCH "Version[ \t]+(4\\.[0-9]+(\\.[0-9]+)?)"
            _ANTLR_BANNER "${_ANTLR_STDOUT}\n${_ANTLR_STDERR}")
        if(_ANTLR_BANNER)
            set(ANTLR_VERSION "${CMAKE_MATCH_1}")
            set(_ANTLR_GENERATOR_WORKS TRUE)
        endif()
    endif()
    if(NOT _ANTLR_GENERATOR_WORKS)
        set(_ANTLR_FAILURE
            "Cannot execute ANTLR 4 generator '${ANTLR_EXECUTABLE}' (${_ANTLR_RESULT}): ${_ANTLR_STDERR}")
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ANTLR
    REQUIRED_VARS ANTLR_EXECUTABLE Java_JAVA_EXECUTABLE _ANTLR_GENERATOR_WORKS
    VERSION_VAR ANTLR_VERSION
    HANDLE_VERSION_RANGE
    REASON_FAILURE_MESSAGE "${_ANTLR_FAILURE}")

function(antlr_target name input_file)
    if(NOT ANTLR_FOUND)
        message(FATAL_ERROR "antlr_target requires a successfully found ANTLR generator")
    endif()
    cmake_parse_arguments(PARSE_ARGV 2 ARG
        "LEXER;PARSER;LISTENER;VISITOR"
        "PACKAGE;OUTPUT_DIRECTORY;DEPENDS_ANTLR"
        "COMPILE_FLAGS;DEPENDS")
    if(ARG_UNPARSED_ARGUMENTS OR ARG_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR
            "Invalid antlr_target(${name}) arguments: ${ARG_UNPARSED_ARGUMENTS} ${ARG_KEYWORDS_MISSING_VALUES}")
    endif()
    if(ARG_LEXER AND NOT ARG_PARSER AND (ARG_LISTENER OR ARG_VISITOR))
        message(FATAL_ERROR "Lexer-only grammars do not generate listeners or visitors")
    endif()

    get_filename_component(input "${input_file}" ABSOLUTE
        BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    get_filename_component(grammar "${input}" NAME_WE)
    if(ARG_OUTPUT_DIRECTORY)
        get_filename_component(output_dir "${ARG_OUTPUT_DIRECTORY}" ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    else()
        set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/antlr4cpp_generated_src/${grammar}")
    endif()

    set(cxx_outputs)
    set(metadata "${output_dir}/${grammar}.interp" "${output_dir}/${grammar}.tokens")
    if((ARG_LEXER AND NOT ARG_PARSER) OR (ARG_PARSER AND NOT ARG_LEXER))
        list(APPEND cxx_outputs "${output_dir}/${grammar}.h" "${output_dir}/${grammar}.cpp")
    else()
        list(APPEND cxx_outputs
            "${output_dir}/${grammar}Lexer.h" "${output_dir}/${grammar}Lexer.cpp"
            "${output_dir}/${grammar}Parser.h" "${output_dir}/${grammar}Parser.cpp")
        list(APPEND metadata
            "${output_dir}/${grammar}Lexer.interp" "${output_dir}/${grammar}Lexer.tokens")
    endif()

    set(generator_options -no-listener -no-visitor)
    foreach(kind IN ITEMS LISTENER VISITOR)
        if(ARG_${kind})
            string(TOLOWER "${kind}" option)
            if(kind STREQUAL "LISTENER")
                set(suffix Listener)
            else()
                set(suffix Visitor)
            endif()
            list(APPEND cxx_outputs
                "${output_dir}/${grammar}Base${suffix}.h"
                "${output_dir}/${grammar}Base${suffix}.cpp"
                "${output_dir}/${grammar}${suffix}.h"
                "${output_dir}/${grammar}${suffix}.cpp")
            list(REMOVE_ITEM generator_options "-no-${option}")
            list(APPEND generator_options "-${option}")
        endif()
    endforeach()
    if(ARG_PACKAGE)
        list(APPEND generator_options -package "${ARG_PACKAGE}")
    endif()
    if(ARG_DEPENDS_ANTLR)
        if(NOT DEFINED ANTLR_${ARG_DEPENDS_ANTLR}_INPUT)
            message(FATAL_ERROR "Unknown ANTLR dependency '${ARG_DEPENDS_ANTLR}'")
        endif()
        list(APPEND ARG_DEPENDS
            "${ANTLR_${ARG_DEPENDS_ANTLR}_INPUT}"
            ${ANTLR_${ARG_DEPENDS_ANTLR}_OUTPUTS})
        list(APPEND generator_options -lib "${ANTLR_${ARG_DEPENDS_ANTLR}_OUTPUT_DIR}")
    endif()

    add_custom_command(
        OUTPUT ${cxx_outputs} ${metadata}
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${output_dir}"
        COMMAND "${Java_JAVA_EXECUTABLE}" -jar "${ANTLR_EXECUTABLE}"
            "${input}" ${ARG_COMPILE_FLAGS} ${generator_options}
            -Dlanguage=Cpp -Xexact-output-dir -o "${output_dir}"
        DEPENDS "${input}" "${ANTLR_EXECUTABLE}" "${Java_JAVA_EXECUTABLE}" ${ARG_DEPENDS}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating ${name} with ANTLR ${ANTLR_VERSION}"
        VERBATIM)
    set(ANTLR_${name}_INPUT "${input}" PARENT_SCOPE)
    set(ANTLR_${name}_OUTPUT_DIR "${output_dir}" PARENT_SCOPE)
    set(ANTLR_${name}_CXX_OUTPUTS "${cxx_outputs}" PARENT_SCOPE)
    set(ANTLR_${name}_OUTPUTS "${cxx_outputs};${metadata}" PARENT_SCOPE)
endfunction()
