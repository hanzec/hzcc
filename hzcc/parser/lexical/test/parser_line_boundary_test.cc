#include <gtest/gtest.h>

#include <string>
#include <string_view>

#include "parser/lexical/test/lexer_test_utils.h"

namespace hzcc::lexical {
namespace {
/** @brief Source spelling and expected token at the end of a source line. */
struct BoundaryTokenCase {
    std::string_view source;
    std::string_view value;
    TokenType type;
};

/** @brief Malformed line-ending literal and its existing diagnostic location. */
struct BoundaryErrorCase {
    std::string_view source;
    std::string_view diagnostic;
    size_t column;
};
}  // namespace

/** @test Line-ending tokens retain their spelling, category and source position. */
TEST(LEXICAL_PARSER, tokens_at_line_end) {
    const BoundaryTokenCase cases[] = {
        {"0", "0", TokenType::Int_Lit},
        {"123", "123", TokenType::Int_Lit},
        {"077", "077", TokenType::Int_Lit},
        {"0xA", "0xA", TokenType::Int_Lit},
        {"0b1", "0b1", TokenType::Int_Lit},
        {"0.5", "0.5", TokenType::Real_Lit},
        {"1.0e-2", "1.0e-2", TokenType::Real_Lit},
        {"3E+10", "3E+10", TokenType::Real_Lit},
        {"name", "name", TokenType::kIdentity},
        {";", ";", TokenType::SemiColon},
        {"}", "}", TokenType::RBrace},
        {"+", "+", TokenType::Add},
        {"/", "/", TokenType::Div},
        {"\"\"", "", TokenType::Str_Lit},
        {"\"text\"", "text", TokenType::Str_Lit},
        {"\"\\n\"", "\n", TokenType::Str_Lit},
        {"'a'", "a", TokenType::Char_Lit},
        {"'\\n'", "\n", TokenType::Char_Lit},
    };

    for (const auto& entry : cases) {
        for (const auto* ending : {"", "\n"}) {
            SCOPED_TRACE(std::string(entry.source) + ending);
            std::stringstream source;
            source << "\n  " << entry.source << ending;
            auto result = ParseTestSource(source);

            ASSERT_TRUE(result.ok()) << result.status();
            ASSERT_EQ(result.value().size(), 1);
            const auto& token = result.value().peek();
            EXPECT_EQ(token.to_str(), entry.value);
            EXPECT_EQ(token.type(), entry.type);
            EXPECT_EQ(token.loc(), Position(2, 2));
        }
    }
}

/** @test Truncated literals return lexical diagnostics without reading past the line. */
TEST(LEXICAL_PARSER, truncated_literals_at_line_end) {
    const BoundaryErrorCase cases[] = {
        {"0x", "Invalid hex string '0x' on integer constant", 2},
        {"0b", "invalid binary number: [0b]", 2},
        {"1e", "invalid integer number", 2},
        {"1e+", "Invalid string '1e+' for real constant", 2},
        {"1.0e", "Invalid string '1.0e' for real constant", 2},
        {"1.0e+", "Invalid string '1.0e+' for real constant", 2},
        {"1.0e-", "Invalid string '1.0e-' for real constant", 2},
        {"\"", "Missing terminating '\"' character", 2},
        {"\"a", "Missing terminating '\"' character", 3},
        {"\"\\", "Missing terminating '\"' character", 3},
        {"\"a\\", "Missing terminating '\"' character", 4},
        {"\"abc\\\"", "Missing terminating '\"' character", 7},
        {"'", "unclosed char literal", 2},
        {"''", "unclosed char literal", 2},
        {"'a", "unclosed char literal", 2},
        {"'\\", "unclosed char literal", 2},
        {"'\\n", "unclosed char literal", 2},
        {"'ab'", "Multi-character character constant", 2},
    };

    for (const auto& entry : cases) {
        for (const auto* ending : {"", "\n"}) {
            SCOPED_TRACE(std::string(entry.source) + ending);
            std::stringstream source;
            source << "\n  " << entry.source << ending;
            testing::internal::CaptureStderr();
            auto result = ParseTestSource(source);
            const auto diagnostic = testing::internal::GetCapturedStderr();

            ASSERT_FALSE(result.ok());
            EXPECT_EQ(result.status().code(), StatusCode::kLexicalAnalysisStage);
            EXPECT_NE(diagnostic.find(entry.diagnostic), std::string::npos);
            EXPECT_NE(diagnostic.find(":2:" + std::to_string(entry.column) + ": "),
                      std::string::npos);
        }
    }
}
}  // namespace hzcc::lexical
