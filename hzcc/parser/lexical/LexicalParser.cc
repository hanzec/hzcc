//
// Created by chen_ on 2022/1/26.
//
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <limits>
#include <magic_enum.hpp>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "macro.h"
#include "parser/common/Token.h"
#include "parser/common/token_type.h"
#include "parser/lexical/utils/symbol_utils.h"
#include "parser/parser.h"
#include "utils/logging.h"
#include "utils/status/status.h"
#include "utils/status/statusor.h"
namespace hzcc::lexical {

namespace {
inline bool regex_match(
    std::string_view sv,  // NOLINT
    const std::regex& e,  // NOLINT
    std::regex_constants::match_flag_type flags = std::regex_constants::match_default) {
    return std::regex_match(sv.data(), sv.data() + sv.size(), e, flags);
}
}  // namespace

#define LIMIT_REGEX_CHECK(REGEX, INPUT, REQUIRED_TYPE, BASE, ERROR_STATUS)          \
    do {                                                                            \
        static std::regex regex(REGEX);                                             \
        if (!regex_match((INPUT), regex)) {                                         \
            ERROR_STATUS;                                                           \
            return {StatusCode::kLexicalAnalysisStage,                              \
                    absl::StrCat("Number :", INPUT,                                 \
                                 "failed verified with regex"                       \
                                 "" #REGEX "")};                                    \
        }                                                                           \
        try {                                                                       \
            if constexpr (std::is_integral_v<REQUIRED_TYPE>) {                      \
                if constexpr (BASE == 2) {                                          \
                    std::stoll(std::string((INPUT).substr(1)), nullptr, BASE);      \
                } else {                                                            \
                    std::stoll(std::string((INPUT)), nullptr, BASE);                \
                }                                                                   \
            } else if constexpr (std::is_floating_point_v<REQUIRED_TYPE>) {         \
                std::stold(std::string((INPUT)));                                   \
            }                                                                       \
        } catch (const std::out_of_range& e) {                                      \
            message::print_message(CompileErrorLevel::Error, "number out of range", \
                                   std::make_pair(ctx->row, start));                \
            return {StatusCode::kLexicalAnalysisStage, "number out of range"};      \
        } catch (const std::invalid_argument& e) {                                  \
            return {StatusCode::kInternal,                                          \
                    "failed to parse string to number even if regex check is "      \
                    " passed"};                                                     \
        }                                                                           \
    } while (0);

typedef struct AnalyzeCtx_t {
    size_t row = 1, col = 0;
    bool multi_line_comment = false;
    std::pair<int, int> multiple_line_comment_start = {0, 0};
} AnalyzeCtx;

ALWAYS_INLINE static Status SymbolHandler(TokenList& tokens, const std::string_view& line,
                                          std::shared_ptr<AnalyzeCtx>& ctx) {
    VLOG(LEXICAL_LOG) << "SymbolHandler: Start at line:" << ctx->row
                      << " col:" << ctx->col;
    auto type = SymbolUtils::GetSymbolType(line.substr(ctx->col, 2));
    /**
     * Here is not add the second symbol to token stream
     * in this loop is because there is a special
     * situation where the comment symbol is after the
     * first symbol like this: [int a = 1; // comment]
     * or [int a = 1; /\* start comment *\/] Which could
     * cause parsing error, there is no need to insert
     * special logic here since only redo one symbol
     * would not influence the performance very much.
     */
    if (magic_enum::enum_integer(type) > 300) {
        // symbols except single char will have value larger then 300
        ctx->col++;  // back to the last digit
    }

    VLOG(LEXICAL_LOG) << "SymbolHandler: Symbol type: [" << magic_enum::enum_name(type)
                      << "]";
    tokens.emplace_back(type, ctx->row, ctx->col);
    return NoError();
}

ALWAYS_INLINE static Status CommentHandler(TokenList& tokens,
                                           const std::string_view& line,
                                           std::shared_ptr<AnalyzeCtx>& ctx) {
    VLOG(LEXICAL_LOG) << "CommentHandler: Start at line:" << ctx->row
                      << " col:" << ctx->col;

    size_t col = ctx->col;
    if (line[col + 1] == '/') {
        VLOG(LEXICAL_LOG) << "CommentHandler: Single line comment";
        ctx->col = line.length();  // skip to the end of line
        return NoError();
    } else if (line[col + 1] == '*') {
        // a special case where multiple line comment is used as
        // single line comment
        if (std::string::npos == line.find("*/", col + 2)) {
            ctx->col = line.length();
            ctx->multi_line_comment = true;
            ctx->multiple_line_comment_start = {ctx->row, col};
            return NoError();
        } else {
            ctx->col = line.find("*/", col + 2) + 1;
        }
    } else {
        HZCC_CHECK_OK_OR_RETURN(SymbolHandler(tokens, line, ctx));
    }
    return NoError();
}

ALWAYS_INLINE static Status StrLitHandler(TokenList& tokens, const std::string_view& line,
                                          std::shared_ptr<AnalyzeCtx>& ctx) {
    VLOG(LEXICAL_LOG) << "StrLitHandler: Start at line:" << ctx->row
                      << " col:" << ctx->col;
    char new_char;
    size_t start = ctx->col;
    std::string tmp_string;

    // collected string literal
    for (ctx->col++; true; ctx->col++) {
        // return error if string literal is not closed
        if (ctx->col > line.length()) {
            message::print_message(CompileErrorLevel::Error,
                                   "Missing terminating '\"' character",
                                   std::make_pair(ctx->row, ctx->col - 2));
            return {StatusCode::kLexicalAnalysisStage,
                    "Missing terminating '\"' character"};
        }

        // handle escape character
        if (line[ctx->col] == '\\') {
            if ((new_char = SymbolUtils::ToASCIIControlCode(line[ctx->col + 1])) !=
                (char)0xFF) {
                ctx->col++;
                tmp_string += new_char;
            } else {
                tmp_string += line[ctx->col];
                tmp_string += line[ctx->col + 1];
                ctx->col++;
            }
        }

        // handle end of string literal
        else if (line[ctx->col] == '"') {
            break;
        }

        // handle other character
        else {
            tmp_string += line[ctx->col];
        }
    }

    // add token
    tokens.emplace_back(TokenType::Str_Lit, tmp_string, ctx->row, start);

    return NoError();
}

ALWAYS_INLINE static Status CharLitHandler(TokenList& tokens,
                                           const std::string_view& line,
                                           std::shared_ptr<AnalyzeCtx>& ctx) {
    VLOG(LEXICAL_LOG) << "CharLitHandler: Start at line:" << ctx->row
                      << " col:" << ctx->col;

    auto row = ctx->row;
    auto start = ctx->col + 1;
    ctx->col = ctx->col + (line[ctx->col + 1] == '\\' ? 3 : 2);

    // if not found '\'' then error
    if (line[ctx->col] != '\'') {
        if (line.find_first_of('\'', ctx->col) == std::string::npos) {
            ctx->col = line.length();
            message::print_message(CompileErrorLevel::Error, "unclosed char literal",
                                   std::make_pair(row, start - 1));
            return {StatusCode::kLexicalAnalysisStage, "unclosed char literal"};
        } else {
            ctx->col += 1;
            message::print_message(CompileErrorLevel::Error,
                                   "Multi-character character constant",
                                   std::make_pair(row, start - 1));
            return {StatusCode::kLexicalAnalysisStage,
                    "Multi-character character constant"};
        }
    } else {
        // handling ASCII escape sequence
        char new_char;
        std::string tmp_string = std::string(line.substr(start, ctx->col - start));
        for (int i = 0; i < tmp_string.length(); ++i) {
            if (tmp_string[i] == '\\' && (new_char = SymbolUtils::ToASCIIControlCode(
                                              tmp_string[i + 1])) != (char)0xFF) {
                tmp_string.replace(i, 2, 1, new_char);
            }
        }

        // add token
        tokens.emplace_back(TokenType::Char_Lit, tmp_string, row, start - 1);
    }

    return NoError();
}

ALWAYS_INLINE static Status NumLitHandler(TokenList& tokens, const std::string_view& line,
                                          std::shared_ptr<AnalyzeCtx>& ctx) {
    VLOG(LEXICAL_LOG) << "NumLitHandler: Start at line:" << ctx->row
                      << " col:" << ctx->col;

    auto start = ctx->col;
    TokenType type = TokenType::Int_Lit;
    ctx->col = line.find_first_not_of("0123456789", start);

    if (ctx->col != std::string::npos) {
        // skip hex literal and binary literal
        if (std::tolower(line[ctx->col]) == 'b' || std::tolower(line[ctx->col]) == 'x') {
            ctx->col++;
        }

        // if real number change type to real
        if (line[ctx->col] == '.') {
            type = TokenType::Real_Lit;
            ctx->col++;
        }
    } else {
        ctx->col = line.length();
    }

    // numbers are ended by a special Symbol or a space,
    // correctness will be checked in syntax test_set_0.
    // Here we want to escape Dot(.) because it could be
    // test_set_0 of real number
    while (ctx->col <= line.length() &&
           (line[ctx->col] == '.' || (!std::isspace(line[ctx->col]) &&
                                      !SymbolUtils::is_operator(line[ctx->col])))) {
        // we want to escape Minus(-) directly after [E]
        // because it could be test_set_0 of real number
        // (e.g. 1.0E-2)
        if (std::tolower(line[ctx->col]) == 'e' &&
            (line[ctx->col + 1] == '-' || line[ctx->col + 1] == '+')) {
            ctx->col++;
            type = TokenType::Real_Lit;
        }
        ctx->col++;
    }

    // do number format check
    auto tmp_numbers = line.substr(start, ctx->col - start);
    if (type == TokenType::Int_Lit) {
        // decimal number
        if (tmp_numbers[0] != '0' ||
            (tmp_numbers[0] == '0' && tmp_numbers.length() == 1)) {
            LIMIT_REGEX_CHECK("^[0-9]+$", tmp_numbers, int64_t, 10, {
                message::print_message(CompileErrorLevel::Error, "invalid integer number",
                                       std::make_pair(ctx->row, start));
            })

        }
        // hex number
        else if (std::tolower(tmp_numbers[1]) == 'x') {
            static std::regex hex_regex("^0[xX][0-9a-fA-F]+$");
            LIMIT_REGEX_CHECK(hex_regex, tmp_numbers, int64_t, 16, {
                auto error_pos =
                    tmp_numbers.find_first_not_of("0123456789abcdefABCDEF", 2);

                if (error_pos == std::string::npos) {
                    message::print_message(CompileErrorLevel::Error,
                                           "Invalid hex string '" +
                                               std::string(tmp_numbers) +
                                               "' on integer constant",
                                           std::make_pair(ctx->row, start));
                } else {
                    message::print_message(
                        CompileErrorLevel::Error,
                        "Invalid suffix '" +
                            std::string(
                                line.substr(error_pos, line.length() - error_pos)) +
                            "' on integer constant",
                        std::make_pair(ctx->row, error_pos));
                }
            })

        }
        // binary number
        else if (std::tolower(tmp_numbers[1]) == 'b') {
            static std::regex binary_regex("^0[bB][01]+$");
            LIMIT_REGEX_CHECK(binary_regex, tmp_numbers, int64_t, 2, {
                message::print_message(
                    CompileErrorLevel::Error,
                    "invalid binary number: [" + std::string(tmp_numbers) + "]",
                    std::make_pair(ctx->row, start));
            })
        }
        // octo number
        else {
            static std::regex octo_regex("^0[0-7]+$");
            LIMIT_REGEX_CHECK(octo_regex, tmp_numbers, int64_t, 8, {
                // looking for which digit is invalid
                auto invalid_digit = tmp_numbers.find_first_not_of("01234567", 1);

                if (invalid_digit == std::string::npos) {
                    message::print_message(CompileErrorLevel::Error,
                                           "Invalid octal string '" +
                                               std::string(tmp_numbers) +
                                               "' on integer constant",
                                           std::make_pair(ctx->row, start));
                } else {
                    message::print_message(
                        CompileErrorLevel::Error,
                        "Invalid digit '" + std::string(1, tmp_numbers[invalid_digit]) +
                            "' in octal constant",
                        std::make_pair(ctx->row, start + invalid_digit));
                }
            })
        }
    } else {
        static std::regex real_number("[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$");
        LIMIT_REGEX_CHECK(real_number, tmp_numbers, long double, 10, {
            auto error_pos = tmp_numbers.find_first_not_of("0123456789.eE+-", 0);

            if (error_pos == std::string::npos) {
                message::print_message(
                    CompileErrorLevel::Error,
                    "Invalid string '" + std::string(tmp_numbers) + "' for real constant",
                    std::make_pair(ctx->row, start));
            } else {
                message::print_message(
                    CompileErrorLevel::Error,
                    "Invalid suffix '" +
                        std::string(line.substr(error_pos, line.length() - error_pos)) +
                        "' on integer constant",
                    std::make_pair(ctx->row, error_pos));
            }
        })

        // turncoat the real number to 10 digits
        auto dot_pos = tmp_numbers.find('.');
        if ((tmp_numbers.length() - dot_pos) >= Options::kMaxSignificantDigits) {
            tmp_numbers =
                tmp_numbers.substr(0, dot_pos + Options::kMaxSignificantDigits + 1);
            message::print_message(
                CompileErrorLevel::Warning,
                absl::StrCat("real number is greater than most possible "
                             "significant digits, will truncating to ",
                             tmp_numbers),
                std::make_pair(ctx->row, start));
        }
    }

    tokens.emplace_back(type, tmp_numbers, ctx->row, start);
    ctx->col--;  // back to the last digit

    return NoError();
}

ALWAYS_INLINE static Status AnalyzeLine(TokenList& tokens, const std::string_view& line,
                                        std::shared_ptr<AnalyzeCtx>& ctx) {
    for (; ctx->col < line.length(); ctx->col++) {
        size_t col = ctx->col;
        switch (line[col]) {
            case ' ':
            case '\t':
            case '\r':
                break;  // skip tab
            case '@':   // skip @
            case '`':
                message::print_message(CompileErrorLevel::Warning,
                                       absl::StrCat("Ignoring unexpected symbol: ",
                                                    std::string(1, line[col])),
                                       std::make_pair(ctx->row, col));
                break;
            case '/': {  // handling comment
                HZCC_CHECK_OK_OR_RETURN(CommentHandler(tokens, line, ctx));
            } break;
            case '"': {  // handling string literal
                HZCC_CHECK_OK_OR_RETURN(StrLitHandler(tokens, line, ctx));
            } break;
            case '\'': {  // handling char literal
                HZCC_CHECK_OK_OR_RETURN(CharLitHandler(tokens, line, ctx));
            } break;
            default: {
                if (std::isdigit(line[col])) {
                    HZCC_CHECK_OK_OR_RETURN(NumLitHandler(tokens, line, ctx));
                } else if (TokenType::kUnknown == SymbolUtils::GetSymbolType(line[col])) {
                    size_t start = col;

                    // find the next closest symbol
                    while (col != std::string::npos && col < line.length() &&
                           !SymbolUtils::is_operator(line[col]))
                        col++;

                    // handling if at line end
                    if (col == std::string::npos) {
                        col = line.length();
                    }

                    auto word = line.substr(start, col - start);
                    auto type = TokenType::kUnknown;

                    // keyword
                    if (TokenType::kUnknown != (type = SymbolUtils::is_keyword(word))) {
                        tokens.emplace_back(type, ctx->row, start);
                    }
                    // type specifier
                    else if (TokenType::kUnknown !=
                             (type = SymbolUtils::is_type_specifier(word))) {
                        tokens.emplace_back(type, ctx->row, start);
                    }
                    // qualifier
                    else if (TokenType::kUnknown !=
                             (type = SymbolUtils::is_qualifier(word))) {
                        tokens.emplace_back(type, ctx->row, start);
                    }
                    // identifier
                    else {
                        tokens.emplace_back(TokenType::kIdentity, word, ctx->row, start);
                    }
                    ctx->col = col - 1;  // back to the last digit
                } else {
                    HZCC_CHECK_OK_OR_RETURN(SymbolHandler(tokens, line, ctx));
                }
            }
        }
    }
    return NoError();
}

StatusOr<TokenList> ParseToToken(std::istream& source) noexcept {
    VLOG(LEXICAL_LOG) << "Start lexical analysis, reading source file";

    TokenList tokens([](const Token& a) -> bool { return is_qualifier(a.type()); });

    std::shared_ptr<AnalyzeCtx> ctx = std::make_shared<AnalyzeCtx>();

    // move to the first line
    source.seekg(0);

    // check stream is valid
    if (!source.good()) {
        message::print_message(CompileErrorLevel::Error, "source file is not readable",
                               std::make_pair(0, 0));
        return NotFoundError("source file not found");
    }

    // read line by line
    for (std::string line; std::getline(source, line); ctx->row++) {
        ctx->col = 0;  // reset col

        // skip empty line
        if (line.empty()) continue;

        VLOG(LEXICAL_LOG) << "Analyzing line " << ctx->row << ": [" << line << "]";

        // detect end of symbol for multiple line comment
        if (ctx->multi_line_comment) {
            if (std::string::npos != (ctx->col = line.find("*/"))) {
                ctx->col += 2;
                ctx->multi_line_comment = false;
            } else {
                continue;
            }
        }

        // analyze current line
        HZCC_CHECK_OK_OR_RETURN(AnalyzeLine(tokens, line, ctx));
    }

    // if line finished without multi-line comment closed
    if (ctx->multi_line_comment) {
        message::print_message(CompileErrorLevel::Error,
                               "Unclosed comment, expected to match this '/*'",
                               ctx->multiple_line_comment_start);
        return Status(StatusCode::kLexicalAnalysisStage,
                      "Unclosed comment at end of file");
    }

    return tokens;
}
}  // namespace hzcc::lexical
