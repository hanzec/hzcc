//
// Created by chen_ on 2022/2/3.
//
#include <gtest/gtest.h>

#include "lexical/utils/symbol_utils.h"

namespace Mycc::Lexical {
TEST(LEXICAL_UTILS_TOKEN_ID, single_char) { // NOLINT
  ASSERT_EQ(SymbolUtils::GetSymbolType('+'),LexicalToken::kAdd);
  ASSERT_EQ(SymbolUtils::GetSymbolType('-'),LexicalToken::kSub);
  ASSERT_EQ(SymbolUtils::GetSymbolType('*'),LexicalToken::kMul);
  ASSERT_EQ(SymbolUtils::GetSymbolType('/'),LexicalToken::kDiv);
  ASSERT_EQ(SymbolUtils::GetSymbolType('%'),LexicalToken::kMod);
  ASSERT_EQ(SymbolUtils::GetSymbolType('='),LexicalToken::kAssign);
  ASSERT_EQ(SymbolUtils::GetSymbolType('!'),LexicalToken::kExclamation);
  ASSERT_EQ(SymbolUtils::GetSymbolType('('),LexicalToken::kLParentheses);
  ASSERT_EQ(SymbolUtils::GetSymbolType(')'),LexicalToken::kRParentheses);
  ASSERT_EQ(SymbolUtils::GetSymbolType('['),LexicalToken::kLBrace);
  ASSERT_EQ(SymbolUtils::GetSymbolType(']'),LexicalToken::kRBrace);
  ASSERT_EQ(SymbolUtils::GetSymbolType(','),LexicalToken::kComma);
  ASSERT_EQ(SymbolUtils::GetSymbolType(';'),LexicalToken::kSemiColon);
  ASSERT_EQ(SymbolUtils::GetSymbolType(':'),LexicalToken::kColon);
  ASSERT_EQ(SymbolUtils::GetSymbolType('.'),LexicalToken::kDot);
  ASSERT_EQ(SymbolUtils::GetSymbolType('>'),LexicalToken::kGreater);
  ASSERT_EQ(SymbolUtils::GetSymbolType('<'),LexicalToken::kLess);
  ASSERT_EQ(SymbolUtils::GetSymbolType('{'),LexicalToken::kLBound);
  ASSERT_EQ(SymbolUtils::GetSymbolType('}'),LexicalToken::kRBound);
  ASSERT_EQ(SymbolUtils::GetSymbolType('?'),LexicalToken::kQuestionMark);
  ASSERT_EQ(SymbolUtils::GetSymbolType('&'),LexicalToken::kAmpersand);
  ASSERT_EQ(SymbolUtils::GetSymbolType('|'),LexicalToken::kVerticalBar);
}

TEST(LEXICAL_UTILS_TOKEN_ID, double_char) { // NOLINT
  ASSERT_EQ(SymbolUtils::GetSymbolType("=="),LexicalToken::kEqual);
  ASSERT_EQ(SymbolUtils::GetSymbolType("!="),LexicalToken::kNotEqual);
  ASSERT_EQ(SymbolUtils::GetSymbolType(">="),LexicalToken::kGreaterEqual);
  ASSERT_EQ(SymbolUtils::GetSymbolType("<="),LexicalToken::kLessEqual);
  ASSERT_EQ(SymbolUtils::GetSymbolType("++"),LexicalToken::kSelfIncrement);
  ASSERT_EQ(SymbolUtils::GetSymbolType("--"),LexicalToken::kSelfDecrement);
  ASSERT_EQ(SymbolUtils::GetSymbolType("||"),LexicalToken::kLogicalOr);
  ASSERT_EQ(SymbolUtils::GetSymbolType("&&"),LexicalToken::kLogicalAnd);
  ASSERT_EQ(SymbolUtils::GetSymbolType("<<"),LexicalToken::kLeftShift);
  ASSERT_EQ(SymbolUtils::GetSymbolType(">>"),LexicalToken::kRightShift);
  ASSERT_EQ(SymbolUtils::GetSymbolType("+="),LexicalToken::kAddAssign);
  ASSERT_EQ(SymbolUtils::GetSymbolType("-="),LexicalToken::kSubAssign);
  ASSERT_EQ(SymbolUtils::GetSymbolType("*="),LexicalToken::kMulAssign);
  ASSERT_EQ(SymbolUtils::GetSymbolType("/="),LexicalToken::kDivAssign);
}

TEST(LEXICAL_UTILS_TOKEN_ID, special_keywords) { // NOLINT
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("const"),LexicalToken::kConst);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("struct"),LexicalToken::kStruct);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("for"),LexicalToken::kFor);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("while"),LexicalToken::kWhile);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("if"),LexicalToken::kIf);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("else"),LexicalToken::kElse);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("return"),LexicalToken::kReturn);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("break"),LexicalToken::kBreak);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("continue"),LexicalToken::kContinue);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("switch"),LexicalToken::kSwitch);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("case"),LexicalToken::kCase);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("default"),LexicalToken::kDefault);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("do"),LexicalToken::kDo);
}
}  // namespace Mycc::Lexical
