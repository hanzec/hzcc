//
// Created by chen_ on 2022/2/3.
//
#include <gtest/gtest.h>

#include "parser/lexical/utils/symbol_utils.h"

namespace hzcc::lexical {
/** @test Single-character punctuation maps to ASCII-valued token categories. */
TEST(LEXICAL_UTILS_TOKEN_ID, single_char) {  // NOLINT
    ASSERT_EQ(SymbolUtils::GetSymbolType('+'), TokenType::Add);
    ASSERT_EQ(SymbolUtils::GetSymbolType('-'), TokenType::Sub);
    ASSERT_EQ(SymbolUtils::GetSymbolType('*'), TokenType::Mul);
    ASSERT_EQ(SymbolUtils::GetSymbolType('/'), TokenType::Div);
    ASSERT_EQ(SymbolUtils::GetSymbolType('%'), TokenType::Mod);
    ASSERT_EQ(SymbolUtils::GetSymbolType('='), TokenType::Assign);
    ASSERT_EQ(SymbolUtils::GetSymbolType('!'), TokenType::LogicalNot);
    ASSERT_EQ(SymbolUtils::GetSymbolType('('), TokenType::LParentheses);
    ASSERT_EQ(SymbolUtils::GetSymbolType(')'), TokenType::RParentheses);
    ASSERT_EQ(SymbolUtils::GetSymbolType('['), TokenType::LBracket);
    ASSERT_EQ(SymbolUtils::GetSymbolType(']'), TokenType::RBracket);
    ASSERT_EQ(SymbolUtils::GetSymbolType(','), TokenType::Comma);
    ASSERT_EQ(SymbolUtils::GetSymbolType(';'), TokenType::SemiColon);
    ASSERT_EQ(SymbolUtils::GetSymbolType(':'), TokenType::Colon);
    ASSERT_EQ(SymbolUtils::GetSymbolType('.'), TokenType::Dot);
    ASSERT_EQ(SymbolUtils::GetSymbolType('>'), TokenType::Greater);
    ASSERT_EQ(SymbolUtils::GetSymbolType('<'), TokenType::Less);
    ASSERT_EQ(SymbolUtils::GetSymbolType('{'), TokenType::LBrace);
    ASSERT_EQ(SymbolUtils::GetSymbolType('}'), TokenType::RBrace);
    ASSERT_EQ(SymbolUtils::GetSymbolType('?'), TokenType::QuestionMark);
    ASSERT_EQ(SymbolUtils::GetSymbolType('&'), TokenType::BitwiseAnd);
    ASSERT_EQ(SymbolUtils::GetSymbolType('|'), TokenType::BitwiseOr);
}

/** @test Compound operators map to their distinct non-ASCII categories. */
TEST(LEXICAL_UTILS_TOKEN_ID, double_char) {  // NOLINT
    ASSERT_EQ(SymbolUtils::GetSymbolType("=="), TokenType::EQ);
    ASSERT_EQ(SymbolUtils::GetSymbolType("!="), TokenType::NE);
    ASSERT_EQ(SymbolUtils::GetSymbolType(">="), TokenType::GE);
    ASSERT_EQ(SymbolUtils::GetSymbolType("<="), TokenType::LE);
    ASSERT_EQ(SymbolUtils::GetSymbolType("++"), TokenType::kSelfIncrement);
    ASSERT_EQ(SymbolUtils::GetSymbolType("--"), TokenType::kSelfDecrement);
    ASSERT_EQ(SymbolUtils::GetSymbolType("||"), TokenType::kLogicalOr);
    ASSERT_EQ(SymbolUtils::GetSymbolType("&&"), TokenType::kLogicalAnd);
    ASSERT_EQ(SymbolUtils::GetSymbolType("<<"), TokenType::kLeftShift);
    ASSERT_EQ(SymbolUtils::GetSymbolType(">>"), TokenType::kRightShift);
    ASSERT_EQ(SymbolUtils::GetSymbolType("+="), TokenType::kAddAssign);
    ASSERT_EQ(SymbolUtils::GetSymbolType("-="), TokenType::kSubAssign);
    ASSERT_EQ(SymbolUtils::GetSymbolType("*="), TokenType::kMulAssign);
    ASSERT_EQ(SymbolUtils::GetSymbolType("/="), TokenType::kDivAssign);
}

/** @test Keyword and qualifier lookup use their respective token ranges. */
TEST(LEXICAL_UTILS_TOKEN_ID, special_keywords) {  // NOLINT
    ASSERT_EQ(SymbolUtils::is_qualifier("const"), TokenType::Const);
    ASSERT_EQ(SymbolUtils::is_keyword("struct"), TokenType::Struct);
    ASSERT_EQ(SymbolUtils::is_keyword("for"), TokenType::For);
    ASSERT_EQ(SymbolUtils::is_keyword("while"), TokenType::While);
    ASSERT_EQ(SymbolUtils::is_keyword("if"), TokenType::If);
    ASSERT_EQ(SymbolUtils::is_keyword("else"), TokenType::Else);
    ASSERT_EQ(SymbolUtils::is_keyword("return"), TokenType::Return);
    ASSERT_EQ(SymbolUtils::is_keyword("break"), TokenType::Break);
    ASSERT_EQ(SymbolUtils::is_keyword("continue"), TokenType::Continue);
    ASSERT_EQ(SymbolUtils::is_keyword("switch"), TokenType::Switch);
    ASSERT_EQ(SymbolUtils::is_keyword("case"), TokenType::Case);
    ASSERT_EQ(SymbolUtils::is_keyword("default"), TokenType::Default);
    ASSERT_EQ(SymbolUtils::is_keyword("do"), TokenType::Do);
}
}  // namespace hzcc::lexical
