//
// Created by chen_ on 2022/2/4.
//
#include <gtest/gtest.h>

#include "parser/lexical/utils/symbol_utils.h"
#include "parser/common/Token.h"

namespace hzcc::lexical::SymbolUtils {
/** @test Supported escape letters decode correctly and other bytes return the sentinel. */
TEST(LEXICAL_UTILS_CONVERTER, ascii_control_code) {  // NOLINT
    ASSERT_EQ('\0', ToASCIIControlCode('0'));
    ASSERT_EQ('\a', ToASCIIControlCode('a'));
    ASSERT_EQ('\b', ToASCIIControlCode('b'));
    ASSERT_EQ('\f', ToASCIIControlCode('f'));
    ASSERT_EQ('\t', ToASCIIControlCode('t'));
    ASSERT_EQ('\n', ToASCIIControlCode('n'));
    ASSERT_EQ('\r', ToASCIIControlCode('r'));
    ASSERT_EQ('\v', ToASCIIControlCode('v'));

    for (size_t i = 0; i <= 0xFF; ++i) {
        if (i == '0' || i == 'a' || i == 'b' || i == 'f' || i == 't' ||
            i == 'n' || i == 'r' || i == 'v') {
            continue;
        }
        ASSERT_EQ((char)0xFF, ToASCIIControlCode((char)i));
    }
}

/** @test Public token spelling renders punctuation, operators, and keyword categories. */
TEST(LEXICAL_UTILS_CONVERTER, token_code_to_string) {
    for (char symbol : parser_common::kSingleCharSymbol) {
        EXPECT_EQ(Token(static_cast<TokenType>(symbol), 1, 0).to_str(),
                  std::string(1, symbol));
    }
    const std::pair<TokenType, const char*> cases[] = {
        {TokenType::EQ, "=="}, {TokenType::NE, "!="},
        {TokenType::GE, ">="}, {TokenType::LE, "<="},
        {TokenType::kSelfIncrement, "++"}, {TokenType::kSelfDecrement, "--"},
        {TokenType::kLogicalOr, "||"}, {TokenType::kLogicalAnd, "&&"},
        {TokenType::kLeftShift, "<<"}, {TokenType::kRightShift, ">>"},
        {TokenType::kAddAssign, "+="}, {TokenType::kSubAssign, "-="},
        {TokenType::kMulAssign, "*="}, {TokenType::kDivAssign, "/="},
        {TokenType::kArrow, "->"}, {TokenType::kModAssign, "%="},
        {TokenType::kAndAssign, "&="}, {TokenType::kOrAssign, "|="},
        {TokenType::kXorAssign, "^="},
        {TokenType::Const, "const"}, {TokenType::Struct, "struct"},
        {TokenType::For, "for"}, {TokenType::While, "while"},
        {TokenType::Do, "do"}, {TokenType::If, "if"}, {TokenType::Else, "else"},
        {TokenType::Break, "break"}, {TokenType::Continue, "continue"},
        {TokenType::Return, "return"}, {TokenType::Switch, "switch"},
        {TokenType::Case, "case"}, {TokenType::Default, "default"},
        {TokenType::Typedef, "typedef"}, {TokenType::Extern, "extern"},
        {TokenType::Goto, "goto"}, {TokenType::Sizeof, "sizeof"},
        {TokenType::Union, "union"}, {TokenType::Enum, "enum"},
        {TokenType::Auto, "auto"}, {TokenType::Volatile, "volatile"},
        {TokenType::Signed, "signed"}, {TokenType::Unsigned, "unsigned"},
        {TokenType::Restrict, "restrict"},
    };
    for (const auto& [category, spelling] : cases) {
        EXPECT_EQ(Token(category, 1, 0).to_str(), spelling);
    }
}
}  // namespace hzcc::lexical::SymbolUtils
