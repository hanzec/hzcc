//
// Created by chen_ on 2022/1/26.
//
#include <regex>

#include "lexical_limit.h"
#include "parser/common/Token.h"
#include "parser/lexical/utils/symbol_utils.h"
#include "parser/parser.h"
#include "utils/logging.h"
#include "utils/status/status.h"
namespace hzcc::lexical {
#define LIMIT_REGEX_CHECK(REGEX, INPUT, REQUIRED_TYPE, ERROR_STATUS)           \
    {                                                                          \
        if (!std::regex_match((INPUT).data(), (REGEX))) {                      \
            do {                                                               \
                ERROR_STATUS                                                   \
            } while (0);                                                       \
        }                                                                      \
        try {                                                                  \
            if constexpr (std::is_same_v<REQUIRED_TYPE, int64_t>) {            \
                if (std::stoll((INPUT).data()) >                               \
                    std::numeric_limits<int64_t>::max()) {                     \
                    message::print_message(kCompileErrorLevel_Error,           \
                                           "integer number overflow",          \
                                           std::make_pair(ctx->row, start));   \
                    return {StatusCode::kLexicalAnalysisStage,                 \
                            "integer number overflow"};                        \
                }                                                              \
            } else if constexpr (std::is_same_v<REQUIRED_TYPE, long double>) { \
                if (std::stod((INPUT).data()) >                                \
                        std::numeric_limits<long double>::max() ||             \
                    std::stod((INPUT).data()) <                                \
                        std::numeric_limits<long double>::lowest()) {          \
                    message::print_message(kCompileErrorLevel_Error,           \
                                           "real number literal out of range", \
                                           std::make_pair(ctx->row, start));   \
                    return {StatusCode::kLexicalAnalysisStage,                 \
                            "real number literal out of range"};               \
                }                                                              \
                                                                               \
                auto dot_pos = tmp_numbers.find('.');                          \
                if (((INPUT).length() - dot_pos) >= kMaxSignificantDigits) {   \
                    auto tmp_str = std::string(INPUT);                         \
                    tmp_str.erase(dot_pos + kMaxSignificantDigits + 1,         \
                                  (INPUT).length());                           \
                    message::print_message(                                    \
                        kCompileErrorLevel_Warning,                            \
                        "real number is greater than most possible "           \
                        "significant digits, will truncating to " +            \
                            tmp_str,                                           \
                        std::make_pair(ctx->row, start));                      \
                }                                                              \
            }                                                                  \
        } catch (const std::out_of_range& e) {                                 \
            message::print_message(kCompileErrorLevel_Error,                   \
                                   "number out of range",                      \
                                   std::make_pair(ctx->row, start));           \
            return {StatusCode::kLexicalAnalysisStage, "number out of range"}; \
        }                                                                      \
    }

typedef struct AnalyzeCtx_t {
    size_t row = 1, col = 0;
    bool multi_line_comment = false;
    std::pair<int, int> multiple_line_comment_start = {0, 0};
} AnalyzeCtx;

ALWAYS_INLINE static Status SymbolHandler(TokenList& tokens,
                                          const std::string_view& line,
                                          std::shared_ptr<AnalyzeCtx>& ctx) {
    auto type = SymbolUtils::GetSymbolType(line.substr(ctx->col, 2).data());
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
    if (magic_enum::enum_integer(type) >
        300) {       // symbols except single char will
                     // have value larger then 300
        ctx->col++;  // back to the last digit
    }
    tokens.emplace_back(type, ctx->row, ctx->col);

    return NoError();
}

ALWAYS_INLINE static Status CommentHandler(TokenList& tokens,
                                           const std::string_view& line,
                                           std::shared_ptr<AnalyzeCtx>& ctx) {
    size_t col = ctx->col;
    if (line[col + 1] == '/') {
        return NoError();
    } else if (line[col + 1] == '*') {
        // a special case where multiple line comment is used as
        // single line comment
        if (std::string::npos == line.find("*/", col + 2)) {
            ctx->multi_line_comment = true;
            ctx->multiple_line_comment_start = {ctx->row - 1, col};
            return NoError();
        } else {
            ctx->col = line.find("*/", col + 2) + 1;
        }
    } else {
        HZCC_CHECK_OK_OR_RETURN(SymbolHandler(tokens, line, ctx));
    }
    return NoError();
}

ALWAYS_INLINE static Status StrLitHandler(TokenList& tokens,
                                          const std::string_view& line,
                                          std::shared_ptr<AnalyzeCtx>& ctx) {
    char new_char;
    size_t start = ctx->col;
    std::string tmp_string;

    // collected string literal
    for (ctx->col++; true; ctx->col++) {
        // return error if string literal is not closed
        if (ctx->col > line.length()) {
            message::print_message(kCompileErrorLevel_Error,
                                   "Missing terminating '\"' character",
                                   std::make_pair(ctx->row, ctx->col - 2));
            return {StatusCode::kLexicalAnalysisStage,
                    "Missing terminating '\"' character"};
        }

        // handle escape character
        if (line[ctx->col] == '\\') {
            if ((new_char = SymbolUtils::ToASCIIControlCode(
                     line[ctx->col + 1])) != (char)0xFF) {
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
    tokens.emplace_back(TokenType::kString, tmp_string, ctx->row, start);

    return NoError();
}

ALWAYS_INLINE static Status CharLitHandler(TokenList& tokens,
                                           const std::string_view& line,
                                           std::shared_ptr<AnalyzeCtx>& ctx) {
    auto row = ctx->row;
    auto start = ctx->col + 1;
    ctx->col = ctx->col + (line[ctx->col + 1] == '\\' ? 3 : 2);

    // if not found '\'' then error
    if (line[ctx->col] != '\'') {
        if (line.find_first_of('\'', ctx->col) == std::string::npos) {
            ctx->col = line.length();
            message::print_message(kCompileErrorLevel_Error,
                                   "unclosed char literal",
                                   std::make_pair(row, start - 1));
            return {StatusCode::kLexicalAnalysisStage, "unclosed char literal"};
        } else {
            ctx->col += 1;
            message::print_message(kCompileErrorLevel_Error,
                                   "Multi-character character constant",
                                   std::make_pair(row, start - 1));
            return {StatusCode::kLexicalAnalysisStage,
                    "Multi-character character constant"};
        }
    } else {
        // handling ASCII escape sequence
        char new_char;
        std::string tmp_string =
            std::string(line.substr(start, ctx->col - start));
        for (int i = 0; i < tmp_string.length(); ++i) {
            if (tmp_string[i] == '\\' &&
                (new_char = SymbolUtils::ToASCIIControlCode(
                     tmp_string[i + 1])) != (char)0xFF) {
                tmp_string.replace(i, 2, 1, new_char);
            }
        }

        // add token
        tokens.emplace_back(TokenType::kChar, tmp_string, row, start - 1);
    }

    return NoError();
}

ALWAYS_INLINE static Status NumLitHandler(TokenList& tokens,
                                          const std::string_view& line,
                                          std::shared_ptr<AnalyzeCtx>& ctx) {
    auto start = ctx->col;
    TokenType type = TokenType::kInteger;
    ctx->col = line.find_first_not_of("0123456789", start);

    if (ctx->col != std::string::npos) {
        // skip hex literal and binary literal
        if (std::tolower(line[ctx->col]) == 'b' ||
            std::tolower(line[ctx->col]) == 'x') {
            ctx->col++;
        }

        // if real number change type to real
        if (line[ctx->col] == '.') {
            type = TokenType::kReal_number;
            ctx->col++;
        }
    } else {
        ctx->col = line.length();
    }

    // numbers are ended by a special Symbol or a space,
    // correctness will be checked in syntax test_set_0.
    // Here we want to escape Dot(.) because it could be
    // test_set_0 of real number
    while (
        ctx->col <= line.length() &&
        (line[ctx->col] == '.' || (!std::isspace(line[ctx->col]) &&
                                   !SymbolUtils::IsOperator(line[ctx->col])))) {
        // we want to escape Minus(-) directly after [E]
        // because it could be test_set_0 of real number
        // (e.g. 1.0E-2)
        if (std::tolower(line[ctx->col]) == 'e' &&
            (line[ctx->col + 1] == '-' || line[ctx->col + 1] == '+')) {
            ctx->col++;
            type = TokenType::kReal_number;
        }
        ctx->col++;
    }

    // do number format check
    auto tmp_numbers = line.substr(start, ctx->col - start);
    if (type == TokenType::kInteger) {
        // decimal number
        if (tmp_numbers[0] != '0' ||
            (tmp_numbers[0] == '0' && tmp_numbers.length() == 1)) {
            static std::regex int_regex("^[0-9]+$");
            LIMIT_REGEX_CHECK(int_regex, tmp_numbers, int64_t, {
                message::print_message(kCompileErrorLevel_Error,
                                       "invalid integer number",
                                       std::make_pair(ctx->row, start));
            })

        }
        // hex number
        else if (std::tolower(tmp_numbers[1]) == 'x') {
            static std::regex hex_regex("^0[xX][0-9a-fA-F]+$");
            LIMIT_REGEX_CHECK(hex_regex, tmp_numbers, int64_t, {
                auto error_pos =
                    tmp_numbers.find_first_not_of("0123456789abcdefABCDEF", 2);

                if (error_pos == std::string::npos) {
                    message::print_message(kCompileErrorLevel_Error,
                                           "Invalid hex string '" +
                                               std::string(tmp_numbers) +
                                               "' on integer constant",
                                           std::make_pair(ctx->row, start));
                } else {
                    message::print_message(
                        kCompileErrorLevel_Error,
                        "Invalid suffix '" +
                            std::string(line.substr(
                                error_pos, line.length() - error_pos)) +
                            "' on integer constant",
                        std::make_pair(ctx->row, error_pos));
                }
            })

        }
        // binary number
        else if (std::tolower(tmp_numbers[1]) == 'b') {
            static std::regex binary_regex("^0[bB][01]+$");
            LIMIT_REGEX_CHECK(binary_regex, tmp_numbers, int64_t, {
                message::print_message(
                    kCompileErrorLevel_Error,
                    "invalid binary number: [" + std::string(tmp_numbers) + "]",
                    std::make_pair(ctx->row, start));
            })
        }
        // octo number
        else {
            static std::regex octo_regex("^0[0-7]+$");
            LIMIT_REGEX_CHECK(octo_regex, tmp_numbers, int64_t, {
                // looking for which digit is invalid
                auto invalid_digit =
                    tmp_numbers.find_first_not_of("01234567", 1);

                if (invalid_digit == std::string::npos) {
                    message::print_message(kCompileErrorLevel_Error,
                                           "Invalid octal string '" +
                                               std::string(tmp_numbers) +
                                               "' on integer constant",
                                           std::make_pair(ctx->row, start));
                } else {
                    message::print_message(
                        kCompileErrorLevel_Error,
                        "Invalid digit '" +
                            std::string(1, tmp_numbers[invalid_digit]) +
                            "' in octal constant",
                        std::make_pair(ctx->row, start + invalid_digit));
                }
            })
        }
    } else {
        static std::regex real_number("[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$");
        LIMIT_REGEX_CHECK(real_number, tmp_numbers, long double, {
            auto error_pos =
                tmp_numbers.find_first_not_of("0123456789.eE+-", 0);

            if (error_pos == std::string::npos) {
                message::print_message(kCompileErrorLevel_Error,
                                       "Invalid string '" +
                                           std::string(tmp_numbers) +
                                           "' for real constant",
                                       std::make_pair(ctx->row, start));
            } else {
                message::print_message(
                    kCompileErrorLevel_Error,
                    "Invalid suffix '" +
                        std::string(
                            line.substr(error_pos, line.length() - error_pos)) +
                        "' on integer constant",
                    std::make_pair(ctx->row, error_pos));
            }
        })
    }

    tokens.emplace_back(type, tmp_numbers, ctx->row, start);
    ctx->col--;  // back to the last digit

    return NoError();
}

ALWAYS_INLINE static Status AnalyzeLine(TokenList& tokens,
                                        const std::string_view& line,
                                        std::shared_ptr<AnalyzeCtx>& ctx) {
    size_t col = ctx->col;
    for (; ctx->col < line.length(); col++) {
        switch (line[col]) {
            case ' ':
            case '\t':
            case '\r':
                break;  // skip tab
            case '@':   // skip @
            case '`':
                message::print_message(kCompileErrorLevel_Warning,
                                       "Unexpected symbol, ignoring.",
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
                } else if (TokenType::kUnknown ==
                           SymbolUtils::GetSymbolType(line[col])) {
                    size_t start = col;

                    // find the next closest symbol
                    while (col != std::string::npos && col < line.length() &&
                           !SymbolUtils::IsOperator(line[col]))
                        col++;

                    // handling if at line end
                    if (col == std::string::npos) {
                        col = line.length();
                    }

                    auto word = line.substr(start, col - start);
                    auto type = SymbolUtils::GetStringKeywordType(word.data());

                    // splitting keyword
                    if (type != TokenType::kUnknown) {
                        tokens.emplace_back(type, ctx->row, start);
                    }
                    // splitting primitive type
                    else if (SymbolUtils::IsPrimitiveType(word.data())) {
                        tokens.emplace_back(TokenType::kType, word, ctx->row,
                                            start);
                    }
                    // identifier
                    else {
                        tokens.emplace_back(TokenType::kIdentity, word,
                                            ctx->row, start);
                    }
                    col--;  // back to the last digit
                } else {
                    HZCC_CHECK_OK_OR_RETURN(SymbolHandler(tokens, line, ctx));
                }
            }
        }
    }
    return NoError();
}

StatusOr<TokenList> ParseToToken(std::istream& source) noexcept {
    TokenList tokens([](const Token& a) -> bool { return a.IsAttribute(); });

    std::shared_ptr<AnalyzeCtx> ctx = std::make_shared<AnalyzeCtx>();

    // move to the first line
    source.seekg(0);

    // check stream is valid
    if (!source.good()) {
        message::print_message(kCompileErrorLevel_Error,
                               "source file is not readable",
                               std::make_pair(0, 0));
        return NotFoundError("source file not found");
    }

    // read line by line
    for (std::string line; std::getline(source, line); ctx->row++) {
        ctx->col = 0;  // reset col

        // skip empty line
        if (line.empty()) continue;

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
        message::print_message(kCompileErrorLevel_Error, "Unclosed comment",
                               ctx->multiple_line_comment_start);
        return Status(StatusCode::kLexicalAnalysisStage, "Unclosed comment");
    }

    return tokens;
}
}  // namespace hzcc::lexical
