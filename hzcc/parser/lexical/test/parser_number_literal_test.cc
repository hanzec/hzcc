//
// Created by chen_ on 2022/2/3.
//
#include <gtest/gtest.h>

#include "parser/common/Token.h"
#include "parser/lexical/test/lexer_test_utils.h"

namespace hzcc::lexical {
TEST(LEXICAL_PARSER, binary_number_0) {  // NOLINT
    std::stringstream os;
    os << "0b111";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "0b111");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Int_Lit);
}

TEST(LEXICAL_PARSER, binary_number_1) {  // NOLINT
    std::stringstream os;
    os << "0b111c";

    auto result = ParseTestSource(os);

    ASSERT_FALSE(result.ok());
    EXPECT_EQ(result.status().code(), StatusCode::kLexicalAnalysisStage);
    EXPECT_FALSE(result.status().message().empty());
}

TEST(LEXICAL_PARSER, binary_number_3) {  // NOLINT
    std::stringstream os;
    os << "c0b111";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "c0b111");
    ASSERT_EQ(token_stream.peek().type(), TokenType::kIdentity);
}

TEST(LEXICAL_PARSER, binary_number_4) {  // NOLINT
    std::stringstream os;
    os << "0b1c11";

    auto result = ParseTestSource(os);

    ASSERT_FALSE(result.ok());
    EXPECT_EQ(result.status().code(), StatusCode::kLexicalAnalysisStage);
    EXPECT_FALSE(result.status().message().empty());
}

TEST(LEXICAL_PARSER, binary_number_5) {  // NOLINT
    std::stringstream os;
    os << "0b11c1 0b111";

    auto result = ParseTestSource(os);

    ASSERT_FALSE(result.ok());
    EXPECT_EQ(result.status().code(), StatusCode::kLexicalAnalysisStage);
    EXPECT_FALSE(result.status().message().empty());
}

TEST(LEXICAL_PARSER, binary_number_6) {  // NOLINT
    std::stringstream os;
    os << "0B11c1 0b111";

    auto result = ParseTestSource(os);

    ASSERT_FALSE(result.ok());
    EXPECT_EQ(result.status().code(), StatusCode::kLexicalAnalysisStage);
    EXPECT_FALSE(result.status().message().empty());
}

TEST(LEXICAL_PARSER, hex_number_1) {  // NOLINT
    std::stringstream os;
    os << "0x333333";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "0x333333");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Int_Lit);
}

TEST(LEXICAL_PARSER, hex_number_2) {  // NOLINT
    std::stringstream os;
    os << "0xDEFGD 0xFFFFFF";

    auto result = ParseTestSource(os);

    ASSERT_FALSE(result.ok());
    EXPECT_EQ(result.status().code(), StatusCode::kLexicalAnalysisStage);
    EXPECT_FALSE(result.status().message().empty());
}

TEST(LEXICAL_PARSER, hex_number_3) {  // NOLINT
    std::stringstream os;
    os << "0X333333";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "0X333333");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Int_Lit);
}

TEST(LEXICAL_PARSER, hex_number_4) {  // NOLINT
    std::stringstream os;
    os << "0xDEFGD 0XFFFFFF";

    auto result = ParseTestSource(os);

    ASSERT_FALSE(result.ok());
    EXPECT_EQ(result.status().code(), StatusCode::kLexicalAnalysisStage);
    EXPECT_FALSE(result.status().message().empty());
}

TEST(LEXICAL_PARSER, real_number_1) {  // NOLINT
    std::stringstream os;
    os << "0.333";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "0.333");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);
}

TEST(LEXICAL_PARSER, real_number_2) {  // NOLINT
    std::stringstream os;
    os << "0.333e-2";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "0.333e-2");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);
}

TEST(LEXICAL_PARSER, real_number_3) {  // NOLINT
    std::stringstream os;
    os << "0.333E-2";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "0.333E-2");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);
}

TEST(LEXICAL_PARSER, real_number_4) {  // NOLINT
    std::stringstream os;
    os << "0.333E-2 0.333e-2 0.333";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 3);
    ASSERT_EQ(token_stream.peek().to_str(), "0.333E-2");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);

    token_stream.pop();
    ASSERT_EQ(token_stream.peek().to_str(), "0.333e-2");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);

    token_stream.pop();
    ASSERT_EQ(token_stream.peek().to_str(), "0.333");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);
}

TEST(LEXICAL_PARSER, real_number_5) {  // NOLINT
    std::stringstream os;
    os << "0.333e+10";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "0.333e+10");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);
}

TEST(LEXICAL_PARSER, real_number_6) {  // NOLINT
    std::stringstream os;
    os << "0.333E+10";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "0.333E+10");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);
}

TEST(LEXICAL_PARSER, real_number_7) {  // NOLINT
    std::stringstream os;
    os << "3E+10";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "3E+10");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);
}

TEST(LEXICAL_PARSER, rinteger_number_1) {  // NOLINT
    std::stringstream os;
    os << "123456789";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 1);
    ASSERT_EQ(token_stream.peek().to_str(), "123456789");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Int_Lit);
}

TEST(LEXICAL_PARSER, rinteger_number_2) {  // NOLINT
    std::stringstream os;
    os << "123456789 123456789";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 2);
    ASSERT_EQ(token_stream.peek().to_str(), "123456789");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Int_Lit);

    token_stream.pop();
    ASSERT_EQ(token_stream.peek().to_str(), "123456789");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Int_Lit);
}

TEST(LEXICAL_PARSER, mixed_number) {  // NOLINT
    std::stringstream os;
    os << "123456789 0.333e-2 0X333333 0.333E-2 0.333 0x333333 0b11111";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(token_stream.size(), 7);
    ASSERT_EQ(token_stream.peek().to_str(), "123456789");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Int_Lit);

    token_stream.pop();
    ASSERT_EQ(token_stream.peek().to_str(), "0.333e-2");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);

    token_stream.pop();
    ASSERT_EQ(token_stream.peek().to_str(), "0X333333");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Int_Lit);

    token_stream.pop();
    ASSERT_EQ(token_stream.peek().to_str(), "0.333E-2");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);

    token_stream.pop();
    ASSERT_EQ(token_stream.peek().to_str(), "0.333");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Real_Lit);

    token_stream.pop();
    ASSERT_EQ(token_stream.peek().to_str(), "0x333333");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Int_Lit);

    token_stream.pop();
    ASSERT_EQ(token_stream.peek().to_str(), "0b11111");
    ASSERT_EQ(token_stream.peek().type(), TokenType::Int_Lit);
}
}  // namespace hzcc::lexical
