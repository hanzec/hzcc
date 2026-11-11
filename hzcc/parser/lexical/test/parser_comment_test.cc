//
// Created by chen_ on 2022/2/1.
//
#include <gtest/gtest.h>

#include <ostream>
#include <vector>

#include "parser/common/Token.h"
#include "parser/lexical/test/lexer_test_utils.h"
#include "parser/lexical/utils/symbol_utils.h"

namespace hzcc::lexical {
/**
 * @brief Verify and consume the five tokens in an integer declaration.
 * @param line_num Zero-based source line used by the original test cases.
 * @param row_offset Column where the declaration starts on its source line.
 * @param result Token stream to verify; it is empty after a successful check.
 */
inline void test_line_1(
    int line_num, int row_offset, TokenList &result) {
    ASSERT_EQ(5, result.size());

    ASSERT_EQ("int", result.peek().to_str());
    ASSERT_EQ(TokenType::Int, result.peek().type());
    ASSERT_EQ(std::make_pair(static_cast<uint64_t>(line_num + 1), static_cast<uint64_t>(row_offset + 0)), result.peek().loc());
    result.pop();

    ASSERT_EQ("a", result.peek().to_str());
    ASSERT_EQ(TokenType::kIdentity, result.peek().type());
    ASSERT_EQ(std::make_pair(static_cast<uint64_t>(line_num + 1), static_cast<uint64_t>(row_offset + 4)), result.peek().loc());
    result.pop();

    ASSERT_EQ("=", result.peek().to_str());
    ASSERT_EQ(TokenType::Assign, result.peek().type());
    ASSERT_EQ(std::make_pair(static_cast<uint64_t>(line_num + 1), static_cast<uint64_t>(row_offset + 6)), result.peek().loc());
    result.pop();

    ASSERT_EQ("1", result.peek().to_str());
    ASSERT_EQ(TokenType::Int_Lit, result.peek().type());
    ASSERT_EQ(std::make_pair(static_cast<uint64_t>(line_num + 1), static_cast<uint64_t>(row_offset + 8)), result.peek().loc());
    result.pop();

    ASSERT_EQ(";", result.peek().to_str());
    ASSERT_EQ(TokenType::SemiColon, result.peek().type());
    ASSERT_EQ(std::make_pair(static_cast<uint64_t>(line_num + 1), static_cast<uint64_t>(row_offset + 9)), result.peek().loc());
    result.pop();
}

TEST(LEXICAL_PARSER, c_style_comment_1) {  // NOLINT
    std::stringstream os;
    os << "// This is a comment\n";
    auto result = ParseTestSource(os);
    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(0, token_stream.size());
}

TEST(LEXICAL_PARSER, c_style_comment_2) {  // NOLINT
    std::stringstream os;
    os << "// This is a comment";
    os << "int a = 1;\n";
    auto result = ParseTestSource(os);
    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    ASSERT_EQ(0, token_stream.size());
}

TEST(LEXICAL_PARSER, c_style_comment_3) {  // NOLINT
    std::stringstream os;
    os << "// This is a comment\n";
    os << "int a = 1;\n";
    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    test_line_1(1, 0, token_stream);
}

TEST(LEXICAL_PARSER, c_style_comment_4) {  // NOLINT
    std::stringstream os;
    os << "int a = 1;\n";
    os << "// This is a comment\n";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    test_line_1(0, 0, token_stream);
}

TEST(LEXICAL_PARSER, c_style_comment_5) {  // NOLINT
    std::stringstream os;
    os << "// This is a comment\n";
    os << "int a = 1;\n";
    os << "// This is a comment\n";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    test_line_1(1, 0, token_stream);
}

TEST(LEXICAL_PARSER, c_style_comment_6) {  // NOLINT
    std::stringstream os;
    os << "// This is a comment\n";
    os << "// This is a comment\n";
    os << "int a = 1;\n";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    test_line_1(2, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_1) {  // NOLINT
    std::stringstream os;
    os << "int a = 1;\n";
    os << "/* This is a comment */\n";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    test_line_1(0, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_2) {  // NOLINT
    std::stringstream os;
    os << "/* This is a comment */\n";
    os << "int a = 1;\n";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    test_line_1(1, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_3) {  // NOLINT
    std::stringstream os;
    os << "/* This is a comment\n */\n";
    os << "int a = 1;\n";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    test_line_1(2, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_4) {  // NOLINT
    std::stringstream os;
    os << "/* This is a comment\n */";
    os << "int a = 1;\n";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    test_line_1(1, 3, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_5) {  // NOLINT
    std::stringstream os;
    os << "int a = 1;";
    os << "/* \nThis is a comment\n */";

    auto result = ParseTestSource(os);

    ASSERT_TRUE(result.ok()) << result.status();
    auto& token_stream = result.value();
    test_line_1(0, 0, token_stream);
}
}  // namespace hzcc::lexical
