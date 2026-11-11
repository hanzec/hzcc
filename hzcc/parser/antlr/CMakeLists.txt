# The experimental ANTLR frontend requires the generator and C++ runtime to match.
set(HZCC_ANTLR_VERSION 4.13.2)
find_package(antlr4-runtime ${HZCC_ANTLR_VERSION} EXACT CONFIG REQUIRED)
find_package(Java 11 COMPONENTS Runtime REQUIRED)

set(ANTLR_EXECUTABLE
    "${CMAKE_CURRENT_BINARY_DIR}/antlr-${HZCC_ANTLR_VERSION}-complete.jar")
file(DOWNLOAD
    "https://www.antlr.org/download/antlr-${HZCC_ANTLR_VERSION}-complete.jar"
    "${ANTLR_EXECUTABLE}"
    EXPECTED_HASH SHA256=eae2dfa119a64327444672aff63e9ec35a20180dc5b8090b7a6ab85125df4d76
    TLS_VERIFY ON
    STATUS antlr_download_status)
list(GET antlr_download_status 0 antlr_download_result)
if(NOT antlr_download_result EQUAL 0)
    message(FATAL_ERROR "ANTLR generator download failed: ${antlr_download_status}")
endif()
find_package(ANTLR ${HZCC_ANTLR_VERSION} EXACT REQUIRED)

antlr_target(antlr4_parser c11.g4
    VISITOR
    PACKAGE hzcc::parser
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/antlr_parser")
add_custom_target(generate_antlr4 DEPENDS ${ANTLR_antlr4_parser_OUTPUTS})

if(TARGET antlr4_static)
    set(hzcc_antlr_runtime antlr4_static)
else()
    set(hzcc_antlr_runtime antlr4_shared)
endif()

add_dependencies(hzcc_core generate_antlr4)
target_sources(hzcc_core PRIVATE ast_listener.cc ${ANTLR_antlr4_parser_CXX_OUTPUTS})
target_include_directories(hzcc_core PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/antlr_parser")
target_link_libraries(hzcc_core PUBLIC ${hzcc_antlr_runtime})
