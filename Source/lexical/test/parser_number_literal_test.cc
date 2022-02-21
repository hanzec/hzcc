//
// Created by chen_ on 2022/2/3.
//
#include <gtest/gtest.h>
#include "lexical/lexical.h"

namespace Mycc::Lexical {
TEST(LEXICAL_PARSER, binary_number_0) {// NOLINT
  std::stringstream os;
  os << "0b111";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "0b111");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, binary_number_1) {// NOLINT
  std::stringstream os;
  os << "0b111c";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_FALSE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "0b111c");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, binary_number_3) {// NOLINT
  std::stringstream os;
  os << "c0b111";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "c0b111");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kIdentity);
}

TEST(LEXICAL_PARSER, binary_number_4) {// NOLINT
  std::stringstream os;
  os << "0b1c11";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_FALSE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "0b1c11");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, binary_number_5) {// NOLINT
  std::stringstream os;
  os << "0b11c1 0b111";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_FALSE(result.Ok());
  ASSERT_EQ(token_stream.size(), 2);
  ASSERT_EQ(token_stream[0].Value(), "0b11c1");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
  ASSERT_EQ(token_stream[1].Value(), "0b111");
  ASSERT_EQ(token_stream[1].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, binary_number_6) {// NOLINT
  std::stringstream os;
  os << "0B11c1 0b111";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_FALSE(result.Ok());
  ASSERT_EQ(token_stream.size(), 2);
  ASSERT_EQ(token_stream[0].Value(), "0B11c1");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
  ASSERT_EQ(token_stream[1].Value(), "0b111");
  ASSERT_EQ(token_stream[1].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, hex_number_1) {// NOLINT
  std::stringstream os;
  os << "0x333333";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "0x333333");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, hex_number_2) {// NOLINT
  std::stringstream os;
  os << "0xDEFGD 0xFFFFFF";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_FALSE(result.Ok());
  ASSERT_EQ(token_stream.size(), 2);
  ASSERT_EQ(token_stream[0].Value(), "0xDEFGD");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
  ASSERT_EQ(token_stream[1].Value(), "0xFFFFFF");
  ASSERT_EQ(token_stream[1].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, hex_number_3) {// NOLINT
  std::stringstream os;
  os << "0X333333";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "0X333333");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, hex_number_4) {// NOLINT
  std::stringstream os;
  os << "0xDEFGD 0XFFFFFF";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_FALSE(result.Ok());
  ASSERT_EQ(token_stream.size(), 2);
  ASSERT_EQ(token_stream[0].Value(), "0xDEFGD");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
  ASSERT_EQ(token_stream[1].Value(), "0XFFFFFF");
  ASSERT_EQ(token_stream[1].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, real_number_1) {// NOLINT
  std::stringstream os;
  os << "0.333";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "0.333");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kReal_number);
}

TEST(LEXICAL_PARSER, real_number_2) {// NOLINT
  std::stringstream os;
  os << "0.333e-2";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "0.333e-2");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kReal_number);
}

TEST(LEXICAL_PARSER, real_number_3) {// NOLINT
  std::stringstream os;
  os << "0.333E-2";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "0.333E-2");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kReal_number);
}

TEST(LEXICAL_PARSER, real_number_4) {// NOLINT
  std::stringstream os;
  os << "0.333E-2 0.333e-2 0.333";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 3);
  ASSERT_EQ(token_stream[0].Value(), "0.333E-2");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kReal_number);
  ASSERT_EQ(token_stream[1].Value(), "0.333e-2");
  ASSERT_EQ(token_stream[1].TokenType(), LexicalToken::kReal_number);
  ASSERT_EQ(token_stream[2].Value(), "0.333");
  ASSERT_EQ(token_stream[2].TokenType(), LexicalToken::kReal_number);
}

TEST(LEXICAL_PARSER, real_number_5) {// NOLINT
  std::stringstream os;
  os << "0.333e+10";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "0.333e+10");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kReal_number);
}

TEST(LEXICAL_PARSER, real_number_6) {// NOLINT
  std::stringstream os;
  os << "0.333E+10";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "0.333E+10");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kReal_number);
}

TEST(LEXICAL_PARSER, real_number_7) {// NOLINT
  std::stringstream os;
  os << "3E+10";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "3E+10");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kReal_number);
}


TEST(LEXICAL_PARSER, rinteger_number_1) {// NOLINT
  std::stringstream os;
  os << "123456789";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 1);
  ASSERT_EQ(token_stream[0].Value(), "123456789");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, rinteger_number_2) {// NOLINT
  std::stringstream os;
  os << "123456789 123456789";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 2);
  ASSERT_EQ(token_stream[0].Value(), "123456789");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
  ASSERT_EQ(token_stream[1].Value(), "123456789");
  ASSERT_EQ(token_stream[1].TokenType(), LexicalToken::kInteger);
}

TEST(LEXICAL_PARSER, mixed_number) {// NOLINT
  std::stringstream os;
  os << "123456789 0.333e-2 0X333333 0.333E-2 0.333 0x333333 0b11111";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  ASSERT_EQ(token_stream.size(), 7);
  ASSERT_EQ(token_stream[0].Value(), "123456789");
  ASSERT_EQ(token_stream[0].TokenType(), LexicalToken::kInteger);
  ASSERT_EQ(token_stream[1].Value(), "0.333e-2");
  ASSERT_EQ(token_stream[1].TokenType(), LexicalToken::kReal_number);
  ASSERT_EQ(token_stream[2].Value(), "0X333333");
  ASSERT_EQ(token_stream[2].TokenType(), LexicalToken::kInteger);
  ASSERT_EQ(token_stream[3].Value(), "0.333E-2");
  ASSERT_EQ(token_stream[3].TokenType(), LexicalToken::kReal_number);
  ASSERT_EQ(token_stream[4].Value(), "0.333");
  ASSERT_EQ(token_stream[4].TokenType(), LexicalToken::kReal_number);
  ASSERT_EQ(token_stream[5].Value(), "0x333333");
  ASSERT_EQ(token_stream[5].TokenType(), LexicalToken::kInteger);
  ASSERT_EQ(token_stream[6].Value(), "0b11111");
  ASSERT_EQ(token_stream[6].TokenType(), LexicalToken::kInteger);
}
}