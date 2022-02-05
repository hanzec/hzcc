//
// Created by chen_ on 2022/2/1.
//
#include <gtest/gtest.h>

#include <ostream>
#include <vector>

#include "lexical/utils/symbol_utils.h"

namespace Mycc::Lexical {
inline __attribute__((always_inline)) void test_line_1(int line_num, int row_offset,
                                                std::vector<LexicalToken> &result) {
  ASSERT_EQ(5, result.size());

  ASSERT_EQ("int", result[0].Value());
  ASSERT_EQ(LexicalToken::kType,result[0].TokenType());
  ASSERT_EQ(std::make_pair(line_num, row_offset + 0), result[0].Location());

  ASSERT_EQ("a", result[1].Value());
  ASSERT_EQ(LexicalToken::kIdentity, result[1].TokenType());
  ASSERT_EQ(std::make_pair(line_num, row_offset + 4), result[1].Location());

  ASSERT_EQ("=", result[2].Value());
  ASSERT_EQ(LexicalToken::kAssign, result[2].TokenType());
  ASSERT_EQ(std::make_pair(line_num, row_offset + 6), result[2].Location());

  ASSERT_EQ("1", result[3].Value());
  ASSERT_EQ(LexicalToken::kInteger, result[3].TokenType());
  ASSERT_EQ(std::make_pair(line_num, row_offset + 8), result[3].Location());

  ASSERT_EQ(";", result[4].Value());
  ASSERT_EQ(LexicalToken::kSemiColon, result[4].TokenType());
  ASSERT_EQ(std::make_pair(line_num, row_offset + 9), result[4].Location());
}

TEST(LEXICAL_PARSER, c_style_comment_1) { // NOLINT
  std::stringstream os;
  os << "// This is a comment\n";
  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);
  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(0, token_stream.size());
}

TEST(LEXICAL_PARSER, c_style_comment_2) { // NOLINT
  std::stringstream os;
  os << "// This is a comment";
  os << "int a = 1;\n";
  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);
  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(0, token_stream.size());
}

TEST(LEXICAL_PARSER, c_style_comment_3) {// NOLINT
  std::stringstream os;
  os << "// This is a comment\n";
  os << "int a = 1;\n";
  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  test_line_1(1, 0, token_stream);
}

TEST(LEXICAL_PARSER, c_style_comment_4) {// NOLINT
  std::stringstream os;
  os << "int a = 1;\n";
  os << "// This is a comment\n";

  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  test_line_1(0, 0, token_stream);
}

TEST(LEXICAL_PARSER, c_style_comment_5) {// NOLINT
  std::stringstream os;
  os << "// This is a comment\n";
  os << "int a = 1;\n";
  os << "// This is a comment\n";

  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  test_line_1(1, 0, token_stream);
}

TEST(LEXICAL_PARSER, c_style_comment_6) {// NOLINT
  std::stringstream os;
  os << "// This is a comment\n";
  os << "// This is a comment\n";
  os << "int a = 1;\n";

  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  test_line_1(2, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_1) {// NOLINT
  std::stringstream os;
  os << "int a = 1;\n";
  os << "/* This is a comment */\n";

  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  test_line_1(0, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_2) {// NOLINT
  std::stringstream os;
  os << "/* This is a comment */\n";
  os << "int a = 1;\n";

  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  test_line_1(1, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_3) {// NOLINT
  std::stringstream os;
  os << "/* This is a comment\n */\n";
  os << "int a = 1;\n";

  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  test_line_1(2, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_4) {// NOLINT
  std::stringstream os;
  os << "/* This is a comment\n */";
  os << "int a = 1;\n";

  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  test_line_1(1, 3, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_5) {// NOLINT
  std::stringstream os;
  os << "int a = 1;";
  os << "/* \nThis is a comment\n */";

  std::vector<Mycc::Lexical::LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = Mycc::Lexical::ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  test_line_1(0, 0, token_stream);
}
}  // namespace Mycc::Lexical
