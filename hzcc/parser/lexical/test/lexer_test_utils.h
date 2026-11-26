#ifndef HZCC_PARSER_LEXICAL_TEST_LEXER_TEST_UTILS_H
#define HZCC_PARSER_LEXICAL_TEST_LEXER_TEST_UTILS_H

#include <sstream>

#include "parser/parser.h"
#include "utils/logging.h"

namespace hzcc::lexical {
/**
 * @brief Tokenize an in-memory test input with a stable diagnostic filename.
 * @param source Seekable stream containing the test's source text.
 * @return Tokens or the lexer error.
 */
inline StatusOr<TokenList> ParseTestSource(std::stringstream& source) {
    message::set_current_file_name("<lexer-test>");
    return ParseToToken(source);
}
}  // namespace hzcc::lexical
#endif
