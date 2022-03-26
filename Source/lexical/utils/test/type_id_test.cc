//
// Created by chen_ on 2022/2/3.
//
#include <gtest/gtest.h>

#include "lexical/utils/symbol_utils.h"

namespace Mycc::Lexical {
TEST(LEXICAL_UTILS_TOKEN_ID, single_char) { // NOLINT
  ASSERT_EQ(SymbolUtils::GetSymbolType('+'),Token::kAdd);
  ASSERT_EQ(SymbolUtils::GetSymbolType('-'),Token::kSub);
  ASSERT_EQ(SymbolUtils::GetSymbolType('*'),Token::kMul);
  ASSERT_EQ(SymbolUtils::GetSymbolType('/'),Token::kDiv);
  ASSERT_EQ(SymbolUtils::GetSymbolType('%'),Token::kMod);
  ASSERT_EQ(SymbolUtils::GetSymbolType('='),Token::kAssign);
  ASSERT_EQ(SymbolUtils::GetSymbolType('!'),Token::kExclamation);
  ASSERT_EQ(SymbolUtils::GetSymbolType('('),Token::kLParentheses);
  ASSERT_EQ(SymbolUtils::GetSymbolType(')'),Token::kRParentheses);
  ASSERT_EQ(SymbolUtils::GetSymbolType('['),Token::kLBrace);
  ASSERT_EQ(SymbolUtils::GetSymbolType(']'),Token::kRBrace);
  ASSERT_EQ(SymbolUtils::GetSymbolType(','),Token::kComma);
  ASSERT_EQ(SymbolUtils::GetSymbolType(';'),Token::kSemiColon);
  ASSERT_EQ(SymbolUtils::GetSymbolType(':'),Token::kColon);
  ASSERT_EQ(SymbolUtils::GetSymbolType('.'),Token::kDot);
  ASSERT_EQ(SymbolUtils::GetSymbolType('>'),Token::kGreater);
  ASSERT_EQ(SymbolUtils::GetSymbolType('<'),Token::kLess);
  ASSERT_EQ(SymbolUtils::GetSymbolType('{'),Token::kLBound);
  ASSERT_EQ(SymbolUtils::GetSymbolType('}'),Token::kRBound);
  ASSERT_EQ(SymbolUtils::GetSymbolType('?'),Token::kQuestionMark);
  ASSERT_EQ(SymbolUtils::GetSymbolType('&'),Token::kAmpersand);
  ASSERT_EQ(SymbolUtils::GetSymbolType('|'),Token::kVerticalBar);
}

TEST(LEXICAL_UTILS_TOKEN_ID, double_char) { // NOLINT
  ASSERT_EQ(SymbolUtils::GetSymbolType("=="),Token::kEqual);
  ASSERT_EQ(SymbolUtils::GetSymbolType("!="),Token::kNotEqual);
  ASSERT_EQ(SymbolUtils::GetSymbolType(">="),Token::kGreaterEqual);
  ASSERT_EQ(SymbolUtils::GetSymbolType("<="),Token::kLessEqual);
  ASSERT_EQ(SymbolUtils::GetSymbolType("++"),Token::kSelfIncrement);
  ASSERT_EQ(SymbolUtils::GetSymbolType("--"),Token::kSelfDecrement);
  ASSERT_EQ(SymbolUtils::GetSymbolType("||"),Token::kLogicalOr);
  ASSERT_EQ(SymbolUtils::GetSymbolType("&&"),Token::kLogicalAnd);
  ASSERT_EQ(SymbolUtils::GetSymbolType("<<"),Token::kLeftShift);
  ASSERT_EQ(SymbolUtils::GetSymbolType(">>"),Token::kRightShift);
  ASSERT_EQ(SymbolUtils::GetSymbolType("+="),Token::kAddAssign);
  ASSERT_EQ(SymbolUtils::GetSymbolType("-="),Token::kSubAssign);
  ASSERT_EQ(SymbolUtils::GetSymbolType("*="),Token::kMulAssign);
  ASSERT_EQ(SymbolUtils::GetSymbolType("/="),Token::kDivAssign);
}

TEST(LEXICAL_UTILS_TOKEN_ID, special_keywords) { // NOLINT
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("const"),Token::kConst);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("struct"),Token::kStruct);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("for"),Token::kFor);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("while"),Token::kWhile);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("if"),Token::kIf);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("else"),Token::kElse);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("return"),Token::kReturn);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("break"),Token::kBreak);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("continue"),Token::kContinue);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("switch"),Token::kSwitch);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("case"),Token::kCase);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("default"),Token::kDefault);
  ASSERT_EQ(SymbolUtils::GetStringKeywordType("do"),Token::kDo);
}
}  // namespace Mycc::Lexical
