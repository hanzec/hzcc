//
// Created by chen_ on 2022/1/26.
//
#include <absl/strings/str_cat.h>
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "macro.h"
#include "parser/common/Token.h"
#include "parser/common/token_type.h"
#include "parser/lexical/utils/symbol_utils.h"
#include "parser/parser.h"
#include "utils/integer_literal.h"
#include "utils/logging.h"
#include "utils/status/status.h"
#include "utils/status/statusor.h"
namespace hzcc::lexical {

/**
 * @brief Mutable lexer position and block-comment state shared across source lines.
 * @note Rows are one-based; columns index the current line from zero.
 */
struct AnalyzeCtx {
    size_t row = 1, col = 0;
    bool multi_line_comment = false;
    std::pair<int, int> multiple_line_comment_start = {0, 0};
    std::string comment_start_line;

    /** @brief Share a spelling among tokens produced by this invocation.
     * @param text Borrowed source text to look up or copy.
     * @return Immutable spelling owned jointly by the tokens and this lexer context.
     */
    std::shared_ptr<const std::string> intern(std::string_view text) {
        const auto found = spellings.find(text);
        if (found != spellings.end()) return found->second;
        auto spelling = std::make_shared<const std::string>(text);
        spellings.emplace(*spelling, spelling);
        return spelling;
    }

  private:
    // Each key points into the string owned by its corresponding mapped value.
    std::unordered_map<std::string_view, std::shared_ptr<const std::string>> spellings;
};

/**
 * @brief Append one punctuation or operator token at the current cursor.
 * @param tokens Destination receiving recognized tokens.
 * @param line Current source line; its backing storage must remain valid during the call.
 * @param ctx Non-null shared lexer state. Cursor points to a symbol; it advances to the last character of a multi-character operator.
 * @return Always OK after appending the classified token.
 */
ALWAYS_INLINE static Status SymbolHandler(TokenList& tokens, const std::string_view& line,
                                          std::shared_ptr<AnalyzeCtx>& ctx) {
    VLOG(LEXICAL_LOG) << "SymbolHandler: Start at line:" << ctx->row
                      << " col:" << ctx->col;
    const auto start = ctx->col;
    auto type = SymbolUtils::GetSymbolType(line.substr(start, 2));
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
    tokens.emplace_back(type, ctx->row, start);
    return NoError();
}

/**
 * @brief Skip a comment or delegate a non-comment slash to the symbol handler.
 * @param tokens Destination receiving recognized tokens.
 * @param line Current source line; its backing storage must remain valid during the call.
 * @param ctx Non-null shared lexer state. Cursor points to a slash; block-comment
 * state may be updated.
 * @return OK after advancing the cursor or processing the slash operator.
 */
ALWAYS_INLINE static Status CommentHandler(TokenList& tokens,
                                           const std::string_view& line,
                                           std::shared_ptr<AnalyzeCtx>& ctx) {
    VLOG(LEXICAL_LOG) << "CommentHandler: Start at line:" << ctx->row
                      << " col:" << ctx->col;

    size_t col = ctx->col;
    if (col + 1 >= line.length()) {
        return SymbolHandler(tokens, line, ctx);
    }
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
            ctx->comment_start_line = line;
            return NoError();
        } else {
            ctx->col = line.find("*/", col + 2) + 1;
        }
    } else {
        HZCC_CHECK_OK_OR_RETURN(SymbolHandler(tokens, line, ctx));
    }
    return NoError();
}

/**
 * @brief Decode a quoted string literal and append its token.
 * @param tokens Destination receiving recognized tokens.
 * @param line Current source line; its backing storage must remain valid during the call.
 * @param ctx Non-null shared lexer state. Cursor points to the opening double quote and advances through the closing quote.
 * @return OK after appending the literal, or a lexical error for a missing closing quote.
 */
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
        if (ctx->col >= line.length() ||
            (line[ctx->col] == '\\' && ctx->col + 1 >= line.length())) {
            message::print_message(CompileErrorLevel::Error,
                                   "Missing terminating '\"' character",
                                   std::make_pair(ctx->row, line.length() - 1), line);
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
    tokens.emplace_back(TokenType::Str_Lit, ctx->intern(tmp_string), ctx->row, start);

    return NoError();
}

/**
 * @brief Decode a single-character literal and append its token.
 * @param tokens Destination receiving recognized tokens.
 * @param line Current source line; its backing storage must remain valid during the call.
 * @param ctx Non-null shared lexer state. Cursor points to the opening single quote and advances to the closing quote.
 * @return OK after appending the literal, or a lexical error for an invalid character constant.
 */
ALWAYS_INLINE static Status CharLitHandler(TokenList& tokens,
                                           const std::string_view& line,
                                           std::shared_ptr<AnalyzeCtx>& ctx) {
    VLOG(LEXICAL_LOG) << "CharLitHandler: Start at line:" << ctx->row
                      << " col:" << ctx->col;

    auto row = ctx->row;
    auto start = ctx->col + 1;
    const bool escaped = start < line.length() && line[start] == '\\';
    ctx->col += escaped ? 3 : 2;

    // if not found '\'' then error
    if (ctx->col >= line.length() || line[ctx->col] != '\'') {
        if (line.find_first_of('\'', ctx->col) == std::string::npos) {
            ctx->col = line.length();
            message::print_message(CompileErrorLevel::Error, "unclosed char literal",
                                   std::make_pair(row, start - 1), line);
            return {StatusCode::kLexicalAnalysisStage, "unclosed char literal"};
        } else {
            ctx->col += 1;
            message::print_message(CompileErrorLevel::Error,
                                   "Multi-character character constant",
                                   std::make_pair(row, start - 1), line);
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
        tokens.emplace_back(TokenType::Char_Lit, ctx->intern(tmp_string), row, start - 1);
    }

    return NoError();
}

/**
 * @brief Scan and validate an integer or real literal, then append its token.
 * @param tokens Destination receiving recognized tokens.
 * @param line Current source line; its backing storage must remain valid during the call.
 * @param ctx Non-null shared lexer state. Cursor points to the first digit or decimal point and
 * advances to the last consumed character.
 * @return OK after appending the literal, or a format, range, or conversion error.
 */
ALWAYS_INLINE static Status NumLitHandler(TokenList& tokens, const std::string_view& line,
                                          std::shared_ptr<AnalyzeCtx>& ctx) {
    VLOG(LEXICAL_LOG) << "NumLitHandler: Start at line:" << ctx->row
                      << " col:" << ctx->col;

    const auto start = ctx->col;
    const auto marker = line.find_first_not_of("0123456789", start);

    // if real number change type to real
    bool real = marker != line.npos && line[marker] == '.';
    if (marker != line.npos && (line[marker] == 'e' || line[marker] == 'E') &&
        marker + 1 < line.size()) {
        const char next = line[marker + 1];
        real =
            std::isdigit(static_cast<unsigned char>(next)) || next == '+' || next == '-';
    }

    // numbers are ended by a special Symbol or a space.
    // Consume the complete candidate so a malformed suffix receives one diagnostic.
    while (ctx->col < line.size()) {
        const char c = line[ctx->col];
        if (std::isspace(static_cast<unsigned char>(c))) break;
        // we want to escape Minus(-) directly after [E]
        // (e.g. 1.0E-2)
        if (c != '.' && SymbolUtils::is_operator(c)) {
            if (!real || (c != '+' && c != '-') || ctx->col == start ||
                (line[ctx->col - 1] != 'e' && line[ctx->col - 1] != 'E'))
                break;
        }
        ++ctx->col;
    }

    // do number format check
    const auto literal = line.substr(start, ctx->col - start);
    const auto error = [&](size_t offset, const std::string& message) {
        message::print_message(CompileErrorLevel::Error, message,
                               {ctx->row, start + offset}, line);
        return Status(StatusCode::kLexicalAnalysisStage, message);
    };
    std::string shortened;
    std::string_view spelling = literal;
    if (real) {
        const auto suffix = literal.find_first_not_of("0123456789.eE+-");
        if (suffix != literal.npos) {
            return error(suffix, absl::StrCat("Invalid suffix '", literal.substr(suffix),
                                              "' on integer constant"));
        }
        size_t consumed = 0;
        try {
            (void)std::stod(std::string(literal), &consumed);
        } catch (const std::out_of_range&) {
            return error(0, "number out of range");
        } catch (const std::invalid_argument&) {
            consumed = 0;
        }
        if (consumed != literal.size()) {
            return error(
                0, absl::StrCat("Invalid string '", literal, "' for real constant"));
        }

        const auto dot = literal.find('.');
        const auto exponent = literal.find_first_of("eE");
        const auto mantissa_end = std::min(exponent, literal.size());
        if (dot != literal.npos) {
            const auto keep = dot + 1 + Options::kMaxSignificantDigits;
            if (keep < mantissa_end) {
                shortened =
                    absl::StrCat(literal.substr(0, keep), literal.substr(mantissa_end));
                spelling = shortened;
                message::print_message(
                    CompileErrorLevel::Warning,
                    absl::StrCat("real number is greater than most possible "
                                 "significant digits, will truncating to ",
                                 spelling),
                    {ctx->row, start}, line);
            }
        }
    } else {
        int64_t value;
        const auto [base, result] = utils::parse_integer_literal(literal, value);
        if (result.ec == std::errc::invalid_argument) {
            const auto offset = static_cast<size_t>(result.ptr - literal.data());
            switch (base) {
                // hex number
                case 16:
                    if (offset == literal.size()) {
                        return error(0, absl::StrCat("Invalid hex string '", literal,
                                                     "' on integer constant"));
                    }
                    return error(offset,
                                 absl::StrCat("Invalid suffix '", literal.substr(offset),
                                              "' on integer constant"));
                // octo number
                case 8:
                    return error(
                        offset, absl::StrCat("Invalid digit '", literal.substr(offset, 1),
                                             "' in octal constant"));
                // binary number
                case 2:
                    return error(0,
                                 absl::StrCat("invalid binary number: [", literal, "]"));
                // decimal number
                default:
                    return error(0, "invalid integer number");
            }
        }
        if (result.ec == std::errc::result_out_of_range) {
            return error(0, "number out of range");
        }
    }

    tokens.emplace_back(real ? TokenType::Real_Lit : TokenType::Int_Lit,
                        ctx->intern(spelling), ctx->row, start);
    --ctx->col;  // back to the last digit

    return NoError();
}

/**
 * @brief Tokenize one source line from the current cursor.
 * @param tokens Destination receiving recognized tokens.
 * @param line Current source line; its backing storage must remain valid during the call.
 * @param ctx Non-null shared lexer state. Cursor advances as tokens and comments are consumed.
 * @return OK after scanning the line, or the first handler error.
 */
ALWAYS_INLINE static Status AnalyzeLine(TokenList& tokens, const std::string_view& line,
                                        std::shared_ptr<AnalyzeCtx>& ctx) {
    for (; ctx->col < line.length(); ctx->col++) {
        size_t col = ctx->col;
        switch (line[col]) {
            case ' ':
            case '\t':
            case '\r':
            case '\f':
            case '\v':
                break;  // skip tab
            case '@':   // skip @
            case '`':
                message::print_message(CompileErrorLevel::Warning,
                                       absl::StrCat("Ignoring unexpected symbol: ",
                                                    std::string(1, line[col])),
                                       std::make_pair(ctx->row, col), line);
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
                if (std::isdigit(static_cast<unsigned char>(line[col])) ||
                    (line[col] == '.' && col + 1 < line.size() &&
                     std::isdigit(static_cast<unsigned char>(line[col + 1])))) {
                    HZCC_CHECK_OK_OR_RETURN(NumLitHandler(tokens, line, ctx));
                } else if (TokenType::kUnknown == SymbolUtils::GetSymbolType(line[col])) {
                    size_t start = col;

                    // find the next closest symbol
                    while (col != std::string::npos && col < line.length() &&
                           !std::isspace(static_cast<unsigned char>(line[col])) &&
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
                        tokens.emplace_back(TokenType::kIdentity, ctx->intern(word),
                                            ctx->row, start);
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

    if (source.bad()) {
        message::print_message(CompileErrorLevel::Error, "Failed to read source stream",
                               {ctx->row, 0}, std::string_view{});
        return InternalError("Failed to read source stream");
    }

    // if line finished without multi-line comment closed
    if (ctx->multi_line_comment) {
        message::print_message(CompileErrorLevel::Error,
                               "Unclosed comment, expected to match this '/*'",
                               ctx->multiple_line_comment_start, ctx->comment_start_line);
        return Status(StatusCode::kLexicalAnalysisStage,
                      "Unclosed comment at end of file");
    }

    return tokens;
}
}  // namespace hzcc::lexical
