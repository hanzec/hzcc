#ifndef HZCC_PARSER_LEXICAL_TEST_LEXER_TEST_UTILS_H
#define HZCC_PARSER_LEXICAL_TEST_LEXER_TEST_UTILS_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "parser/parser.h"
#include "utils/logging.h"

namespace hzcc::lexical {
/**
 * @brief Tokenize test input with a matching on-disk source for diagnostics.
 * @param source Seekable stream containing the test's source text.
 * @return Tokens or the lexer error; temporary diagnostic input is removed.
 * @note The parser reads the stream, while diagnostics read the temporary file.
 */
inline StatusOr<TokenList> ParseTestSource(std::stringstream& source) {
    const auto path = std::filesystem::temp_directory_path() /
        ("hzcc-lexer-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()) + ".c");
    {
        std::ofstream file(path);
        if (!(file << source.str())) {
            return InternalError("Cannot create lexer diagnostic input");
        }
    }
    message::set_current_file_name(path.string());
    auto result = ParseToToken(source);
    std::filesystem::remove(path);
    return result;
}
}  // namespace hzcc::lexical
#endif
