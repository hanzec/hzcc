//
// Created by chen_ on 2022/2/2.
//

#include <gtest/gtest.h>

#include <ostream>
#include <vector>

#include "lexical/lexical.h"

namespace Mycc::Lexical {
TEST(LEXICAL_PARSER, string_iteral_1) {
  std::stringstream os;
  os << "\"Hello, World!\"";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  EXPECT_EQ(token_stream.size(), 1);
  EXPECT_EQ(token_stream[0].TokenType(), LexicalToken::kString);
  EXPECT_EQ(token_stream[0].Value(), "Hello, World!");
}

TEST(LEXICAL_PARSER, string_iteral_2) {
  std::stringstream os;
  os << R"("Hello, world!\n")";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  EXPECT_EQ(token_stream.size(), 1);
  EXPECT_EQ(token_stream[0].TokenType(), LexicalToken::kString);
  EXPECT_EQ(token_stream[0].Value(), "Hello, world!\n");
}

TEST(LEXICAL_PARSER, string_iteral_3) {
  std::stringstream os;
  os << R"("I said, \"Hello, world!\"")";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  EXPECT_EQ(token_stream.size(), 1);
  EXPECT_EQ(token_stream[0].TokenType(), LexicalToken::kString);
  EXPECT_EQ(token_stream[0].Value(), "I said, \\\"Hello, world!\\\"");
}

TEST(LEXICAL_PARSER, string_iteral_4) {
  std::stringstream os;
  os << R"("This is evil \\")";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_TRUE(result.Ok());
  EXPECT_EQ(token_stream.size(), 1);
  EXPECT_EQ(token_stream[0].TokenType(), LexicalToken::kString);
  EXPECT_EQ(token_stream[0].Value(), "This is evil \\\\");
}

TEST(LEXICAL_PARSER, string_iteral_5) {
  std::stringstream os;
  os << R"("This is evil \\)";

  std::vector<LexicalToken> token_stream;
  token_stream.reserve(20);
  auto result = ParseToToken(os, token_stream);

  ASSERT_FALSE(result.Ok());
  EXPECT_EQ(token_stream.size(), 1);
  EXPECT_EQ(token_stream[0].TokenType(), LexicalToken::kString);
  EXPECT_EQ(token_stream[0].Value(), "This is evil \\\\");
}

}