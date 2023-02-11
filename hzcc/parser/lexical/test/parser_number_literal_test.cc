//
// Created by chen_ on 2022/2/3.
//
#include <gtest/gtest.h>

#include "parser/common/Token.h"
#include "parser/lexical.h"

namespace hzcc::lexical {
TEST(LEXICAL_PARSER, binary_number_0) {  // NOLINT
    std::stringstream os;
    os << "0b111";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "0b111");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, binary_number_1) {  // NOLINT
    std::stringstream os;
    os << "0b111c";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_FALSE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "0b111c");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, binary_number_3) {  // NOLINT
    std::stringstream os;
    os << "c0b111";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "c0b111");
    ASSERT_EQ(token_stream.front().type(), kIdentity);
}

TEST(LEXICAL_PARSER, binary_number_4) {  // NOLINT
    std::stringstream os;
    os << "0b1c11";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_FALSE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "0b1c11");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, binary_number_5) {  // NOLINT
    std::stringstream os;
    os << "0b11c1 0b111";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_FALSE(result.Ok());
    ASSERT_EQ(token_stream.size(), 2);
    ASSERT_EQ(token_stream.front().to_str(), "0b11c1");
    ASSERT_EQ(token_stream.front().type(), kInteger);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0b111");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, binary_number_6) {  // NOLINT
    std::stringstream os;
    os << "0B11c1 0b111";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_FALSE(result.Ok());
    ASSERT_EQ(token_stream.size(), 2);
    ASSERT_EQ(token_stream.front().to_str(), "0B11c1");
    ASSERT_EQ(token_stream.front().type(), kInteger);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0b111");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, hex_number_1) {  // NOLINT
    std::stringstream os;
    os << "0x333333";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "0x333333");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, hex_number_2) {  // NOLINT
    std::stringstream os;
    os << "0xDEFGD 0xFFFFFF";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_FALSE(result.Ok());
    ASSERT_EQ(token_stream.size(), 2);
    ASSERT_EQ(token_stream.front().to_str(), "0xDEFGD");
    ASSERT_EQ(token_stream.front().type(), kInteger);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0xFFFFFF");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, hex_number_3) {  // NOLINT
    std::stringstream os;
    os << "0X333333";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "0X333333");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, hex_number_4) {  // NOLINT
    std::stringstream os;
    os << "0xDEFGD 0XFFFFFF";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_FALSE(result.Ok());
    ASSERT_EQ(token_stream.size(), 2);
    ASSERT_EQ(token_stream.front().to_str(), "0xDEFGD");
    ASSERT_EQ(token_stream.front().type(), kInteger);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0XFFFFFF");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, real_number_1) {  // NOLINT
    std::stringstream os;
    os << "0.333";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "0.333");
    ASSERT_EQ(token_stream.front().type(), kReal_number);
}

TEST(LEXICAL_PARSER, real_number_2) {  // NOLINT
    std::stringstream os;
    os << "0.333e-2";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "0.333e-2");
    ASSERT_EQ(token_stream.front().type(), kReal_number);
}

TEST(LEXICAL_PARSER, real_number_3) {  // NOLINT
    std::stringstream os;
    os << "0.333E-2";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "0.333E-2");
    ASSERT_EQ(token_stream.front().type(), kReal_number);
}

TEST(LEXICAL_PARSER, real_number_4) {  // NOLINT
    std::stringstream os;
    os << "0.333E-2 0.333e-2 0.333";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 3);
    ASSERT_EQ(token_stream.front().to_str(), "0.333E-2");
    ASSERT_EQ(token_stream.front().type(), kReal_number);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0.333e-2");
    ASSERT_EQ(token_stream.front().type(), kReal_number);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0.333");
    ASSERT_EQ(token_stream.front().type(), kReal_number);
}

TEST(LEXICAL_PARSER, real_number_5) {  // NOLINT
    std::stringstream os;
    os << "0.333e+10";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "0.333e+10");
    ASSERT_EQ(token_stream.front().type(), kReal_number);
}

TEST(LEXICAL_PARSER, real_number_6) {  // NOLINT
    std::stringstream os;
    os << "0.333E+10";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "0.333E+10");
    ASSERT_EQ(token_stream.front().type(), kReal_number);
}

TEST(LEXICAL_PARSER, real_number_7) {  // NOLINT
    std::stringstream os;
    os << "3E+10";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "3E+10");
    ASSERT_EQ(token_stream.front().type(), kReal_number);
}

TEST(LEXICAL_PARSER, rinteger_number_1) {  // NOLINT
    std::stringstream os;
    os << "123456789";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.front().to_str(), "123456789");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, rinteger_number_2) {  // NOLINT
    std::stringstream os;
    os << "123456789 123456789";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 2);
    ASSERT_EQ(token_stream.front().to_str(), "123456789");
    ASSERT_EQ(token_stream.front().type(), kInteger);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "123456789");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}

TEST(LEXICAL_PARSER, mixed_number) {  // NOLINT
    std::stringstream os;
    os << "123456789 0.333e-2 0X333333 0.333E-2 0.333 0x333333 0b11111";

    std::list<Token> token_stream;
    auto result = ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(token_stream.size(), 7);
    ASSERT_EQ(token_stream.front().to_str(), "123456789");
    ASSERT_EQ(token_stream.front().type(), kInteger);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0.333e-2");
    ASSERT_EQ(token_stream.front().type(), kReal_number);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0X333333");
    ASSERT_EQ(token_stream.front().type(), kInteger);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0.333E-2");
    ASSERT_EQ(token_stream.front().type(), kReal_number);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0.333");
    ASSERT_EQ(token_stream.front().type(), kReal_number);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0x333333");
    ASSERT_EQ(token_stream.front().type(), kInteger);

    token_stream.pop_front();
    ASSERT_EQ(token_stream.front().to_str(), "0b11111");
    ASSERT_EQ(token_stream.front().type(), kInteger);
}
}  // namespace hzcc::lexical
