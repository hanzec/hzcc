//
// Created by chen_ on 2022/2/3.
//
#include <gtest/gtest.h>

#include "lexical/utils/symbol_utils.h"

namespace Hzcc::Lexical {
TEST(LEXICAL_UTILS_TOKEN_ID, single_char) {  // NOLINT
    ASSERT_EQ(SymbolUtils::GetSymbolType('+'), kAdd);
    ASSERT_EQ(SymbolUtils::GetSymbolType('-'), kSub);
    ASSERT_EQ(SymbolUtils::GetSymbolType('*'), kMul);
    ASSERT_EQ(SymbolUtils::GetSymbolType('/'), kDiv);
    ASSERT_EQ(SymbolUtils::GetSymbolType('%'), kMod);
    ASSERT_EQ(SymbolUtils::GetSymbolType('='), kAssign);
    ASSERT_EQ(SymbolUtils::GetSymbolType('!'), kLogicalNot);
    ASSERT_EQ(SymbolUtils::GetSymbolType('('), kLParentheses);
    ASSERT_EQ(SymbolUtils::GetSymbolType(')'), kRParentheses);
    ASSERT_EQ(SymbolUtils::GetSymbolType('['), kLBracket);
    ASSERT_EQ(SymbolUtils::GetSymbolType(']'), kRBracket);
    ASSERT_EQ(SymbolUtils::GetSymbolType(','), kComma);
    ASSERT_EQ(SymbolUtils::GetSymbolType(';'), kSemiColon);
    ASSERT_EQ(SymbolUtils::GetSymbolType(':'), kColon);
    ASSERT_EQ(SymbolUtils::GetSymbolType('.'), kDot);
    ASSERT_EQ(SymbolUtils::GetSymbolType('>'), kGreater);
    ASSERT_EQ(SymbolUtils::GetSymbolType('<'), kLess);
    ASSERT_EQ(SymbolUtils::GetSymbolType('{'), kLBrace);
    ASSERT_EQ(SymbolUtils::GetSymbolType('}'), kRBrace);
    ASSERT_EQ(SymbolUtils::GetSymbolType('?'), kQuestionMark);
    ASSERT_EQ(SymbolUtils::GetSymbolType('&'), kBitwiseAnd);
    ASSERT_EQ(SymbolUtils::GetSymbolType('|'), kBitwiseOr);
}

TEST(LEXICAL_UTILS_TOKEN_ID, double_char) {  // NOLINT
    ASSERT_EQ(SymbolUtils::GetSymbolType("=="), kEqual);
    ASSERT_EQ(SymbolUtils::GetSymbolType("!="), kNotEqual);
    ASSERT_EQ(SymbolUtils::GetSymbolType(">="), kGreaterEqual);
    ASSERT_EQ(SymbolUtils::GetSymbolType("<="), kLessEqual);
    ASSERT_EQ(SymbolUtils::GetSymbolType("++"), kSelfIncrement);
    ASSERT_EQ(SymbolUtils::GetSymbolType("--"), kSelfDecrement);
    ASSERT_EQ(SymbolUtils::GetSymbolType("||"), kLogicalOr);
    ASSERT_EQ(SymbolUtils::GetSymbolType("&&"), kLogicalAnd);
    ASSERT_EQ(SymbolUtils::GetSymbolType("<<"), kLeftShift);
    ASSERT_EQ(SymbolUtils::GetSymbolType(">>"), kRightShift);
    ASSERT_EQ(SymbolUtils::GetSymbolType("+="), kAddAssign);
    ASSERT_EQ(SymbolUtils::GetSymbolType("-="), kSubAssign);
    ASSERT_EQ(SymbolUtils::GetSymbolType("*="), kMulAssign);
    ASSERT_EQ(SymbolUtils::GetSymbolType("/="), kDivAssign);
}

TEST(LEXICAL_UTILS_TOKEN_ID, special_keywords) {  // NOLINT
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("const"), kConst);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("struct"), kStruct);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("for"), kFor);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("while"), kWhile);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("if"), kIf);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("else"), kElse);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("return"), kReturn);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("break"), kBreak);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("continue"), kContinue);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("switch"), kSwitch);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("case"), kCase);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("default"), kDefault);
    ASSERT_EQ(SymbolUtils::GetStringKeywordType("do"), kDo);
}
}  // namespace Hzcc::Lexical
