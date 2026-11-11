//
// Created by chen_ on 2022/2/2.
//

#include <gtest/gtest.h>

#include <ostream>
#include <vector>

#include "parser/common/Token.h"
#include "parser/lexical/test/lexer_test_utils.h"

namespace hzcc::lexical {
TEST(LEXICAL_PARSER, string_iteral_1) {
    std::stringstream os;
    os << "\"Hello, World!\"";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    EXPECT_EQ(token_stream.size(), 1);
    EXPECT_EQ(token_stream.peek().type(), TokenType::Str_Lit);
    EXPECT_EQ(token_stream.peek().to_str(), "Hello, World!");
}

TEST(LEXICAL_PARSER, string_iteral_2) {
    std::stringstream os;
    os << R"("Hello, world!\n")";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    EXPECT_EQ(token_stream.size(), 1);
    EXPECT_EQ(token_stream.peek().type(), TokenType::Str_Lit);
    EXPECT_EQ(token_stream.peek().to_str(), "Hello, world!\n");
}

TEST(LEXICAL_PARSER, string_iteral_3) {
    std::stringstream os;
    os << R"("I said, \"Hello, world!\"")";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    EXPECT_EQ(token_stream.size(), 1);
    EXPECT_EQ(token_stream.peek().type(), TokenType::Str_Lit);
    EXPECT_EQ(token_stream.peek().to_str(), "I said, \\\"Hello, world!\\\"");
}

TEST(LEXICAL_PARSER, string_iteral_4) {
    std::stringstream os;
    os << R"("This is evil \\")";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    EXPECT_EQ(token_stream.size(), 1);
    EXPECT_EQ(token_stream.peek().type(), TokenType::Str_Lit);
    EXPECT_EQ(token_stream.peek().to_str(), "This is evil \\\\");
}

TEST(LEXICAL_PARSER, string_iteral_5) {
    std::stringstream os;
    os << R"("This is evil \\)";

    auto result = ParseTestSource(os);

    ASSERT_FALSE(result.ok());
    EXPECT_EQ(result.status().code(), StatusCode::kLexicalAnalysisStage);
    EXPECT_FALSE(result.status().message().empty());
}

}  // namespace hzcc::lexical
