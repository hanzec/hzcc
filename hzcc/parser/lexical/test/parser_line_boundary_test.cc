#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
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
/** @brief A seekable input buffer that fails after delivering one complete line. */
class FailingSourceBuffer : public std::stringbuf {
  public:
    /** @brief Supply an initial tokenizable line before the simulated read failure. */
    FailingSourceBuffer() : std::stringbuf("int ok;\n", std::ios::in) {}

  protected:
    /** @brief Signal a read error when the stream requests the next character. */
    int_type underflow() override {
        throw std::ios_base::failure("simulated read failure");
    }
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
        {"1e3", "1e3", TokenType::Real_Lit},
        {".5", ".5", TokenType::Real_Lit},
        {"2.", "2.", TokenType::Real_Lit},
        {"1.E4", "1.E4", TokenType::Real_Lit},
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

/** @test A lexer diagnostic retains its actual source line without a backing file. */
TEST(LEXICAL_PARSER, diagnostic_source_line_from_stream) {
    std::stringstream source("int ok;\n/* open\nstill open");
    testing::internal::CaptureStderr();
    auto result = ParseTestSource(source);
    const auto diagnostic = testing::internal::GetCapturedStderr();
    EXPECT_FALSE(result.ok());
    EXPECT_NE(diagnostic.find("<lexer-test>:2:0:"), std::string::npos);
    EXPECT_NE(diagnostic.find("/* open"), std::string::npos);
    EXPECT_EQ(diagnostic.find("column number is less than 1"), std::string::npos);
}

/** @test Source excerpts are available for private, read-only files and optional at EOF.
 */
TEST(LEXICAL_PARSER, diagnostic_read_only_source) {
    namespace fs = std::filesystem;
    const auto path =
        fs::path(testing::TempDir()) /
        ("hzcc-private-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    ASSERT_TRUE(std::ofstream(path) << "source excerpt\n");
    const auto previous = message::getFilename();
    message::set_current_file_name(path.string());
    for (auto permissions :
         {fs::perms::owner_read | fs::perms::owner_write, fs::perms::owner_read}) {
        fs::permissions(path, permissions);
        testing::internal::CaptureStderr();
        message::print_message(CompileErrorLevel::Error, "test message", {1, 0});
        const auto diagnostic = testing::internal::GetCapturedStderr();
        EXPECT_NE(diagnostic.find("source excerpt"), std::string::npos);
        EXPECT_NE(diagnostic.find(":1:0:"), std::string::npos);
    }
    testing::internal::CaptureStderr();
    message::print_message(CompileErrorLevel::Error, "past end", {3, 4});
    const auto past_end = testing::internal::GetCapturedStderr();
    EXPECT_NE(past_end.find("past end"), std::string::npos);
    EXPECT_EQ(past_end.find(" | "), std::string::npos);
    fs::permissions(path, fs::perms::owner_write, fs::perm_options::add);
    fs::remove(path);
    testing::internal::CaptureStderr();
    message::print_message(CompileErrorLevel::Error, "missing source", {1, 0});
    const auto diagnostic = testing::internal::GetCapturedStderr();
    EXPECT_NE(diagnostic.find("missing source"), std::string::npos);
    EXPECT_EQ(diagnostic.find(" | "), std::string::npos);
    message::set_current_file_name(previous);
}

/** @test A failed read after valid input identifies the source and discards its tokens.
 */
TEST(LEXICAL_PARSER, read_failure_reports_filename) {
    FailingSourceBuffer buffer;
    std::istream source(&buffer);
    const auto previous = message::getFilename();
    message::set_current_file_name("<interrupted>");
    testing::internal::CaptureStderr();
    auto result = ParseToToken(source);
    const auto diagnostic = testing::internal::GetCapturedStderr();
    message::set_current_file_name(previous);

    EXPECT_TRUE(source.bad());
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.status().code(), StatusCode::kInternal);
    EXPECT_NE(diagnostic.find("<interrupted>:2:0:"), std::string::npos);
    EXPECT_EQ(diagnostic.find(" | "), std::string::npos);
    EXPECT_NE(diagnostic.find("Failed to read source stream"), std::string::npos);
}

/** @test Clipping fractional digits preserves the exponent and its numeric scale. */
TEST(LEXICAL_PARSER, real_precision_preserves_exponent) {
    std::stringstream source("1.23456789012345678901234e-2 1.234567890123456789e-2");
    testing::internal::CaptureStderr();
    auto result = ParseTestSource(source);
    const auto diagnostic = testing::internal::GetCapturedStderr();
    ASSERT_TRUE(result.ok()) << result.status();
    ASSERT_EQ(result.value().size(), 2);
    const auto shortened = result.value().pop();
    const auto other = result.value().pop();
    EXPECT_EQ(shortened.type(), TokenType::Real_Lit);
    EXPECT_EQ(other.type(), TokenType::Real_Lit);
    EXPECT_EQ(shortened.to_str(), "1.2345678901234567890e-2");
    EXPECT_DOUBLE_EQ(std::stod(shortened.to_str()), std::stod(other.to_str()));
    EXPECT_NE(diagnostic.find("truncating to " + shortened.to_str()), std::string::npos);
}

/** @test An exponent sign is local to a real literal and a lone dot remains punctuation.
 */
TEST(LEXICAL_PARSER, number_boundaries) {
    std::stringstream source("0x1e+2 . .25e-1");
    auto result = ParseTestSource(source);
    ASSERT_TRUE(result.ok()) << result.status();
    for (const auto* spelling : {"0x1e", "+", "2", ".", ".25e-1"}) {
        ASSERT_FALSE(result.value().empty());
        EXPECT_EQ(result.value().pop().to_str(), spelling);
    }
    EXPECT_TRUE(result.value().empty());
}
}  // namespace hzcc::lexical
