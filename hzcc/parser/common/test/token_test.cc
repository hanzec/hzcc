#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "parser/common/Token.h"

namespace hzcc::parser_common {

/** @test A token owns its spelling independently of the source string lifetime. */
TEST(PARSER_COMMON_TOKEN, construct_one_with_string_test) {
    std::string source = "test";
    Token token(TokenType::kIdentity, source, 1, 0);
    source.assign("changed");

    EXPECT_EQ(token.to_str(), "test");
    EXPECT_EQ(token.type(), TokenType::kIdentity);
    EXPECT_EQ(token.loc(), (Position{1, 0}));
}

/** @test A punctuation token derives its spelling from its category. */
TEST(PARSER_COMMON_TOKEN, construct_one_without_string_test) {
    Token token(TokenType::Add, 1, 3);
    EXPECT_EQ(token.to_str(), "+");
    EXPECT_EQ(token.type(), TokenType::Add);
    EXPECT_EQ(token.loc(), (Position{1, 3}));
}

/** @test Equal spellings survive destruction of another token that shares them. */
TEST(PARSER_COMMON_TOKEN, construct_two_with_same_string_test) {
    auto first = std::make_unique<Token>(TokenType::kIdentity, "test", 1, 0);
    Token second(TokenType::kIdentity, "test", 1, 1);
    EXPECT_EQ(first->to_str(), second.to_str());
    EXPECT_EQ(second.type(), TokenType::kIdentity);
    EXPECT_EQ(second.loc(), (Position{1, 1}));
    first.reset();
    EXPECT_EQ(second.to_str(), "test");
}

/** @test Different interned spellings remain distinct across copies and destruction. */
TEST(PARSER_COMMON_TOKEN, construct_three_string_test) {
    auto first = std::make_unique<Token>(TokenType::kIdentity, "test", 1, 0);
    Token other(TokenType::kIdentity, "test_other", 1, 1);
    Token copy = *first;
    first.reset();

    EXPECT_EQ(copy.to_str(), "test");
    EXPECT_EQ(copy.type(), TokenType::kIdentity);
    EXPECT_EQ(copy.loc(), (Position{1, 0}));
    EXPECT_EQ(other.to_str(), "test_other");
    EXPECT_EQ(other.loc(), (Position{1, 1}));
}

/** @test Changing a token category preserves its location and stored spelling. */
TEST(PARSER_COMMON_TOKEN, change_category) {
    Token token(TokenType::kIdentity, "example", 2, 5);
    token.Type(TokenType::Str_Lit);
    EXPECT_EQ(token.type(), TokenType::Str_Lit);
    EXPECT_EQ(token.loc(), (Position{2, 5}));
    EXPECT_EQ(token.to_str(), "example");
    EXPECT_EQ(token.to_str(true), "\"example\"");
}

/** @test Equality compares category and location rather than token spelling. */
TEST(PARSER_COMMON_TOKEN, equality) {
    Token first(TokenType::kIdentity, "first", 2, 0);
    Token same_position(TokenType::kIdentity, "second", 2, 0);
    Token next_column(TokenType::kIdentity, "first", 2, 1);
    EXPECT_TRUE(first == same_position);
    EXPECT_FALSE(first == next_column);
}

}  // namespace hzcc::parser_common
