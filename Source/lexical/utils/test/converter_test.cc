//
// Created by chen_ on 2022/2/4.
//
#include <gtest/gtest.h>

#include "lexical/utils/symbol_utils.h"

namespace Mycc::Lexical::SymbolUtils {
TEST(LEXICAL_UTILS_CONVERTER, ascii_control_code) { // NOLINT
  ASSERT_EQ('\0', ToASCIIControlCode('0'));
  ASSERT_EQ('\a', ToASCIIControlCode('a'));
  ASSERT_EQ('\b', ToASCIIControlCode('b'));
  ASSERT_EQ('\f', ToASCIIControlCode('f'));
  ASSERT_EQ('\t', ToASCIIControlCode('t'));
  ASSERT_EQ('\n', ToASCIIControlCode('n'));
  ASSERT_EQ('\r', ToASCIIControlCode('r'));
  ASSERT_EQ('\v', ToASCIIControlCode('v'));

  for (size_t i = 0; i <= 0xFF; ++i) {
    if (i == '0' || i == 'a' || i == 'b' || i == 'f' || i == 't' || i == 'n' || i == 'r' ||
        i == 'v') {
      continue;
    }
    ASSERT_EQ((char)0xFF, ToASCIIControlCode((char)i));
  }
}

TEST(LEXICAL_UTILS_CONVERTER, token_code_to_string) { // NOLINT
  /**
 * Special Symbol's table.
   */
   const std::array<const char, kSingleCharSymbolTableSize> singleCharSymbol{
      '+', '-', '*', '/', '%', '&', '|', '!', '=', '.', ',',
      '<', '>', '?', ':', ';', '{', '}', '(', ')', '[', ']'};

  const std::array<const char *, kDoubleCharSymbolTableSize> doubleCharSymbol{
      "==", "!=", ">=", "<=", "++", "--", "||", "&&", "<<", ">>", "+=", "-=", "*=", "/="};

  /**
 * Special Keyword's table.
   */
  const std::array<const char *, kKeywordTableSize> arrayKeyword{
      "const", "struct",   "for",    "while",  "do",   "if",      "else",
      "break", "continue", "return", "switch", "case", "default", "typedef"
  };

  // single char symbols
  for (auto & symbol: singleCharSymbol) {
    ASSERT_EQ(std::string(1,symbol), TokenTypeToString((LexicalToken::Type)symbol));
  }

  // double char symbols
  for (int i = 0; i < doubleCharSymbol.size(); ++i) {
    ASSERT_EQ(doubleCharSymbol[i], TokenTypeToString((LexicalToken::Type)(i + 351)));
  }

  // keywords
  for (int i = 0; i < arrayKeyword.size(); ++i) {
    ASSERT_EQ(arrayKeyword[i], TokenTypeToString((LexicalToken::Type)(i + 401)));
  }
}
}  // namespace Mycc::Lexical::SymbolUtils