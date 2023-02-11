//
// Created by chen_ on 2022/2/1.
//
#include <gtest/gtest.h>

#include <ostream>
#include <vector>

#include "parser/common/Token.h"
#include "parser/lexical.h"
#include "parser/lexical/utils/symbol_utils.h"

namespace hzcc::lexical {
inline __attribute__((always_inline)) void test_line_1(
    int line_num, int row_offset, std::list<Token> &result) {
    ASSERT_EQ(5, result.size());

    ASSERT_EQ("int", result.front().to_str());
    ASSERT_EQ(kType, result.front().type());
    ASSERT_EQ(std::make_pair(line_num, row_offset + 0), result.front().loc());
    result.pop_front();

    ASSERT_EQ("a", result.front().to_str());
    ASSERT_EQ(kIdentity, result.front().type());
    ASSERT_EQ(std::make_pair(line_num, row_offset + 4), result.front().loc());
    result.pop_front();

    ASSERT_EQ("=", result.front().to_str());
    ASSERT_EQ(kTokenType_Assign, result.front().type());
    ASSERT_EQ(std::make_pair(line_num, row_offset + 6), result.front().loc());
    result.pop_front();

    ASSERT_EQ("1", result.front().to_str());
    ASSERT_EQ(kInteger, result.front().type());
    ASSERT_EQ(std::make_pair(line_num, row_offset + 8), result.front().loc());
    result.pop_front();

    ASSERT_EQ(";", result.front().to_str());
    ASSERT_EQ(kTokenType_SemiColon, result.front().type());
    ASSERT_EQ(std::make_pair(line_num, row_offset + 9), result.front().loc());
    result.pop_front();
}

TEST(LEXICAL_PARSER, c_style_comment_1) {  // NOLINT
    std::stringstream os;
    os << "// This is a comment\n";
    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);
    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(0, token_stream.size());
}

TEST(LEXICAL_PARSER, c_style_comment_2) {  // NOLINT
    std::stringstream os;
    os << "// This is a comment";
    os << "int a = 1;\n";
    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);
    ASSERT_TRUE(result.Ok());
    ASSERT_EQ(0, token_stream.size());
}

TEST(LEXICAL_PARSER, c_style_comment_3) {  // NOLINT
    std::stringstream os;
    os << "// This is a comment\n";
    os << "int a = 1;\n";
    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    test_line_1(1, 0, token_stream);
}

TEST(LEXICAL_PARSER, c_style_comment_4) {  // NOLINT
    std::stringstream os;
    os << "int a = 1;\n";
    os << "// This is a comment\n";

    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    test_line_1(0, 0, token_stream);
}

TEST(LEXICAL_PARSER, c_style_comment_5) {  // NOLINT
    std::stringstream os;
    os << "// This is a comment\n";
    os << "int a = 1;\n";
    os << "// This is a comment\n";

    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    test_line_1(1, 0, token_stream);
}

TEST(LEXICAL_PARSER, c_style_comment_6) {  // NOLINT
    std::stringstream os;
    os << "// This is a comment\n";
    os << "// This is a comment\n";
    os << "int a = 1;\n";

    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    test_line_1(2, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_1) {  // NOLINT
    std::stringstream os;
    os << "int a = 1;\n";
    os << "/* This is a comment */\n";

    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    test_line_1(0, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_2) {  // NOLINT
    std::stringstream os;
    os << "/* This is a comment */\n";
    os << "int a = 1;\n";

    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    test_line_1(1, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_3) {  // NOLINT
    std::stringstream os;
    os << "/* This is a comment\n */\n";
    os << "int a = 1;\n";

    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    test_line_1(2, 0, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_4) {  // NOLINT
    std::stringstream os;
    os << "/* This is a comment\n */";
    os << "int a = 1;\n";

    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    test_line_1(1, 3, token_stream);
}

TEST(LEXICAL_PARSER, cpp_style_comment_5) {  // NOLINT
    std::stringstream os;
    os << "int a = 1;";
    os << "/* \nThis is a comment\n */";

    std::list<Token> token_stream;
    auto result = hzcc::lexical::ParseToToken(os);

    ASSERT_TRUE(result.Ok());
    test_line_1(0, 0, token_stream);
}
}  // namespace hzcc::lexical
