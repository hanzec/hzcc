//
// Created by chen_ on 2022/2/2.
//

#include <gtest/gtest.h>

#include <ostream>
#include <vector>

#include "parser/common/Token.h"
#include "parser/lexical.h"

namespace hzcc::lexical {
TEST(LEXICAL_PARSER, string_iteral_1) {
    std::stringstream os;
    os << "\"Hello, World!\"";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    EXPECT_EQ(token_stream.size(), 1);
    EXPECT_EQ(token_stream.front().Type(), kString);
    EXPECT_EQ(token_stream.front().val(), "Hello, World!");
}

TEST(LEXICAL_PARSER, string_iteral_2) {
    std::stringstream os;
    os << R"("Hello, world!\n")";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    EXPECT_EQ(token_stream.size(), 1);
    EXPECT_EQ(token_stream.front().Type(), kString);
    EXPECT_EQ(token_stream.front().val(), "Hello, world!\n");
}

TEST(LEXICAL_PARSER, string_iteral_3) {
    std::stringstream os;
    os << R"("I said, \"Hello, world!\"")";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    EXPECT_EQ(token_stream.size(), 1);
    EXPECT_EQ(token_stream.front().Type(), kString);
    EXPECT_EQ(token_stream.front().val(), "I said, \\\"Hello, world!\\\"");
}

TEST(LEXICAL_PARSER, string_iteral_4) {
    std::stringstream os;
    os << R"("This is evil \\")";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    EXPECT_EQ(token_stream.size(), 1);
    EXPECT_EQ(token_stream.front().Type(), kString);
    EXPECT_EQ(token_stream.front().val(), "This is evil \\\\");
}

TEST(LEXICAL_PARSER, string_iteral_5) {
    std::stringstream os;
    os << R"("This is evil \\)";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_FALSE(result.Ok());
    EXPECT_EQ(token_stream.size(), 1);
    EXPECT_EQ(token_stream.front().Type(), kString);
    EXPECT_EQ(token_stream.front().val(), "This is evil \\\\");
}

}  // namespace hzcc::lexical
