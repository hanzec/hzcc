//
// Created by chen_ on 2022/1/26.
//
#include <regex>

#include "lexical.h"
#include "lexical/Token.h"
#include "lexical_limit.h"
#include "utils/Status.h"
#include "utils/message_utils.h"
#include "utils/symbol_utils.h"

namespace Hzcc::Lexical {
#define CHECK_BY_REGEX(CHECK_REGEX, CHECK_STRING, REQUIRED_TYPE, PRINT_ERROR)  \
    {                                                                          \
        if (!std::regex_match((CHECK_STRING), (CHECK_REGEX))) {                \
            { PRINT_ERROR }                                                    \
            return {Status::INTERNAL, "Parsing error"};                        \
        }                                                                      \
                                                                               \
        try {                                                                  \
            if constexpr (std::is_same_v<REQUIRED_TYPE, int64_t>) {            \
                if (std::stoll((CHECK_STRING)) >                               \
                    std::numeric_limits<int64_t>::max()) {                     \
                    Message::print_message(Message::kError,                    \
                                           "integer number overflow", line,    \
                                           std::make_pair(row, start));        \
                    return {Status::INTERNAL, "Parsing error"};                \
                }                                                              \
            } else if constexpr (std::is_same_v<REQUIRED_TYPE, long double>) { \
                if (std::stod((CHECK_STRING)) >                                \
                        std::numeric_limits<long double>::max() ||             \
                    std::stod((CHECK_STRING)) <                                \
                        std::numeric_limits<long double>::lowest()) {          \
                    Message::print_message(Message::kError,                    \
                                           "real number literal out of range", \
                                           line, std::make_pair(row, start));  \
                    return {Status::INTERNAL, "Parsing error"};                \
                }                                                              \
                                                                               \
                auto dot_pos = tmp_numbers.find('.');                          \
                if (((CHECK_STRING).length() - dot_pos) >=                     \
                    kMaxSignificantDigits) {                                   \
                    (CHECK_STRING)                                             \
                        .erase(dot_pos + kMaxSignificantDigits + 1,            \
                               (CHECK_STRING).length());                       \
                    Message::print_message(                                    \
                        Message::kWarning,                                     \
                        "real number is greater than most possible "           \
                        "significant digits, will truncating to " +            \
                            (CHECK_STRING),                                    \
                        line, std::make_pair(row, start));                     \
                }                                                              \
            }                                                                  \
        } catch (const std::out_of_range& e) {                                 \
            Message::print_message(Message::kError, "number out of range",     \
                                   line, std::make_pair(row, start));          \
            return {Status::INTERNAL, "Parsing error"};                        \
        }                                                                      \
    }

Status ParseToToken(std::istream& source, std::list<Token>& tokens) {
    // move to the first line
    source.seekg(0);

    // check stream is valid
    if (!source.good()) {
        return {Status::INVALID_ARGUMENT, "source stream is invalid"};
    }

    // read line by line
    size_t row = 1;
    bool multiple_line_comment = false;
    std::pair<int, int> multiple_line_comment_start{0, 0};
    for (std::string line; std::getline(source, line); row++) {
        size_t col = 0;

        // skip empty line
        if (line.empty()) continue;

        // detect end of symbol for multiple line comment
        if (multiple_line_comment) {
            if (std::string::npos != (col = line.find("*/"))) {
                col += 2;
                multiple_line_comment = false;
            } else {
                continue;
            }
        }

        // analyze current line
        for (; col < line.length(); col++) {
            switch (line[col]) {
                case ' ':
                case '\t':
                case '\r':
                    break;  // skip tab
                case '@':   // skip @
                case '`':
                    Message::print_message(Message::kWarning,
                                           "Unexpected symbol, ignoring.", line,
                                           std::make_pair(row, col));
                    break;
                case '/': {  // handling comment
                    if (line[col + 1] == '/') {
                        goto NEXT_LINE;  // skip line
                    } else if (line[col + 1] == '*') {
                        // a special case where multiple line comment is used as
                        // single line comment
                        if (std::string::npos == line.find("*/", col + 2)) {
                            multiple_line_comment = true;
                            multiple_line_comment_start = {row - 1, col};
                            goto NEXT_LINE;
                        } else {
                            col = line.find("*/", col + 2) + 1;
                        }
                    } else {
                        goto HANDLE_SYMBOL;  // direct to handle symbol case
                    }
                } break;
                case '"': {  // handling string literal
                    char new_char;
                    size_t start = col;
                    std::string tmp_string;

                    // collected string literal
                    for (col++; true; col++) {
                        // return error if string literal is not closed
                        if (col > line.length()) {
                            Message::print_message(
                                Message::kError,
                                "Missing terminating '\"' character", line,
                                std::make_pair(row, col - 2));
                            return {Status::INTERNAL,
                                    "Missing terminating '\"' character"};
                        }

                        // handle escape character
                        if (line[col] == '\\') {
                            if ((new_char = SymbolUtils::ToASCIIControlCode(
                                     line[col + 1])) != (char)0xFF) {
                                col++;
                                tmp_string += new_char;
                            } else {
                                tmp_string += line[col];
                                tmp_string += line[col + 1];
                                col++;
                            }
                        }

                        // handle end of string literal
                        else if (line[col] == '"') {
                            break;
                        }

                        // handle other character
                        else {
                            tmp_string += line[col];
                        }
                    }

                    // add token
                    tokens.emplace_back(tmp_string, TokenType::kString, row,
                                        start, line);
                } break;
                case '\'': {  // handling char literal
                    auto start = col + 1;
                    col = col + (line[col + 1] == '\\' ? 3 : 2);

                    // if not found '\'' then error
                    if (line[col] != '\'') {
                        if (line.find_first_of('\'', col) ==
                            std::string::npos) {
                            col = line.length();
                            Message::print_message(
                                Message::kError, "unclosed char literal", line,
                                std::make_pair(row, start - 1));
                            return {Status::INTERNAL, "unclosed char literal"};
                        } else {
                            col += 1;
                            Message::print_message(
                                Message::kError,
                                "Multi-character character constant", line,
                                std::make_pair(row, start - 1));
                            return {Status::INTERNAL,
                                    "Multi-character character constant"};
                        }
                    } else {
                        // handling ASCII escape sequence
                        char new_char;
                        auto tmp_string = line.substr(start, col - start);
                        for (int i = 0; i < tmp_string.length(); ++i) {
                            if (tmp_string[i] == '\\' &&
                                (new_char = SymbolUtils::ToASCIIControlCode(
                                     tmp_string[i + 1])) != (char)0xFF) {
                                tmp_string.replace(i, 2, 1, new_char);
                            }
                        }

                        // add token
                        tokens.emplace_back(tmp_string, TokenType::kChar, row,
                                            start - 1, line);
                    }
                } break;
                default: {
                    if (std::isdigit(line[col])) {
                        auto start = col;
                        TokenType type = TokenType::kInteger;
                        col = line.find_first_not_of("0123456789", start);

                        if (col != std::string::npos) {
                            // skip hex literal and binary literal
                            if (std::tolower(line[col]) == 'b' ||
                                std::tolower(line[col]) == 'x') {
                                col++;
                            }

                            // if real number change type to real
                            if (line[col] == '.') {
                                type = TokenType::kReal_number;
                                col++;
                            }
                        } else {
                            col = line.length();
                        }

                        // numbers are ended by a special Symbol or a space,
                        // correctness will be checked in syntax test_set_0.
                        // Here we want to escape Dot(.) because it could be
                        // test_set_0 of real number
                        while (col <= line.length() &&
                               (line[col] == '.' ||
                                (!std::isspace(line[col]) &&
                                 !SymbolUtils::IsOperator(line[col])))) {
                            // we want to escape Minus(-) directly after [E]
                            // because it could be test_set_0 of real number
                            // (e.g. 1.0E-2)
                            if (std::tolower(line[col]) == 'e' &&
                                (line[col + 1] == '-' ||
                                 line[col + 1] == '+')) {
                                col++;
                                type = TokenType::kReal_number;
                            }
                            col++;
                        }

                        // do number format check
                        auto tmp_numbers = line.substr(start, col - start);
                        if (type == TokenType::kInteger) {
                            // decimal number
                            if (tmp_numbers[0] != '0' ||
                                (tmp_numbers[0] == '0' &&
                                 tmp_numbers.length() == 1)) {
                                static std::regex int_regex("^[0-9]+$");
                                CHECK_BY_REGEX(
                                    int_regex, tmp_numbers, int64_t, {
                                        Message::print_message(
                                            Message::kError,
                                            "invalid integer number", line,
                                            std::make_pair(row, start));
                                    })

                            }
                            // hex number
                            else if (std::tolower(tmp_numbers[1]) == 'x') {
                                static std::regex hex_regex(
                                    "^0[xX][0-9a-fA-F]+$");
                                CHECK_BY_REGEX(
                                    hex_regex, tmp_numbers, int64_t, {
                                        auto error_pos =
                                            tmp_numbers.find_first_not_of(
                                                "0123456789abcdefABCDEF", 2);

                                        if (error_pos == std::string::npos) {
                                            Message::print_message(
                                                Message::kError,
                                                "Invalid hex string '" +
                                                    tmp_numbers +
                                                    "' on integer constant",
                                                line,
                                                std::make_pair(row, start));
                                        } else {
                                            Message::print_message(
                                                Message::kError,
                                                "Invalid suffix '" +
                                                    line.substr(error_pos,
                                                                line.length() -
                                                                    error_pos) +
                                                    "' on integer constant",
                                                line,
                                                std::make_pair(row, error_pos));
                                        }
                                    })

                            }
                            // binary number
                            else if (std::tolower(tmp_numbers[1]) == 'b') {
                                static std::regex binary_regex("^0[bB][01]+$");
                                CHECK_BY_REGEX(
                                    binary_regex, tmp_numbers, int64_t, {
                                        Message::print_message(
                                            Message::kError,
                                            "invalid binary number: [" +
                                                tmp_numbers + "]",
                                            line, std::make_pair(row, start));
                                    })
                            }
                            // octo number
                            else {
                                static std::regex octo_regex("^0[0-7]+$");
                                CHECK_BY_REGEX(
                                    octo_regex, tmp_numbers, int64_t, {
                                        // looking for which digit is invalid
                                        auto invalid_digit =
                                            tmp_numbers.find_first_not_of(
                                                "01234567", 1);

                                        if (invalid_digit ==
                                            std::string::npos) {
                                            Message::print_message(
                                                Message::kError,
                                                "Invalid octal string '" +
                                                    tmp_numbers +
                                                    "' on integer constant",
                                                line,
                                                std::make_pair(row, start));
                                        } else {
                                            Message::print_message(
                                                Message::kError,
                                                "Invalid digit '" +
                                                    std::string(
                                                        1,
                                                        tmp_numbers
                                                            [invalid_digit]) +
                                                    "' in octal constant",
                                                line,
                                                std::make_pair(
                                                    row,
                                                    start + invalid_digit));
                                        }
                                    })
                            }
                        } else {
                            static std::regex real_number(
                                "[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$");
                            CHECK_BY_REGEX(
                                real_number, tmp_numbers, long double, {
                                    auto error_pos =
                                        tmp_numbers.find_first_not_of(
                                            "0123456789.eE+-", 0);

                                    if (error_pos == std::string::npos) {
                                        Message::print_message(
                                            Message::kError,
                                            "Invalid string '" + tmp_numbers +
                                                "' for real constant",
                                            line, std::make_pair(row, start));
                                    } else {
                                        Message::print_message(
                                            Message::kError,
                                            "Invalid suffix '" +
                                                line.substr(
                                                    error_pos,
                                                    line.length() - error_pos) +
                                                "' on integer constant",
                                            line,
                                            std::make_pair(row, error_pos));
                                    }
                                })
                        }

                        tokens.emplace_back(tmp_numbers, type, row, start,
                                            line);
                        col--;  // back to the last digit
                    } else if (TokenType::kUnknown ==
                               SymbolUtils::GetSymbolType(line[col])) {
                        size_t start = col;

                        // find the next closest symbol
                        while (col != std::string::npos &&
                               col < line.length() &&
                               !SymbolUtils::IsOperator(line[col]))
                            col++;

                        // handling if at line end
                        if (col == std::string::npos) {
                            col = line.length();
                        }

                        auto word = line.substr(start, col - start);
                        auto type = SymbolUtils::GetStringKeywordType(word);

                        // splitting keyword
                        if (type != TokenType::kUnknown) {
                            tokens.emplace_back(type, row, start, line);
                        }
                        // splitting primitive type
                        else if (SymbolUtils::IsPrimitiveType(word.c_str())) {
                            tokens.emplace_back(word, TokenType::kType, row,
                                                start, line);
                        }
                        // identifier
                        else {
                            tokens.emplace_back(word, TokenType::kIdentity, row,
                                                start, line);
                        }
                        col--;  // back to the last digit
                    } else {
                    HANDLE_SYMBOL:
                        auto type = SymbolUtils::GetSymbolType(
                            line.substr(col, 2).c_str());
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
                        if (type > 300) {  // symbols except single char will
                                           // have value larger then 300
                            col++;         // back to the last digit
                        }
                        tokens.emplace_back(type, row, col, line);
                    }
                }
            }
        }
    NEXT_LINE:;
    }

    // if line finished without multi-line comment closed
    if (multiple_line_comment) {
        Message::print_message(Message::kError, "Unclosed comment",
                               multiple_line_comment_start);
        return {Status::INTERNAL, "Unclosed comment"};
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Lexical
