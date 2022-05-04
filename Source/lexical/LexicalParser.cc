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

static std::regex kHex_number_regex("^0[xX][0-9a-fA-F]+$");
static std::regex kOct_number_regex("^0[0-7]+$");
static std::regex kInteger_number_regex("^[0-9]+$");
static std::regex kBinary_number_regex("^0[bB][01]+$");
static std::regex kReal_number_regex("[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$");

Status ParseToToken(std::istream& source, std::list<Token>& tokens) {
    bool have_error = false;

    // move to the first line
    source.seekg(0);

    // check stream is valid
    if (!source.good()) {
        return {Status::INVALID_ARGUMENT, "source stream is invalid"};
    }

    // read line by line
    int row = 0;
    bool multiple_line_comment = false;
    std::pair<int, int> multiple_line_comment_start{0, 0};
    for (std::string line; std::getline(source, line); row++) {
        size_t col = 0;  // current index in line by default is 0

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
        for (const char* line_buf = line.c_str(); col < line.length(); col++) {
            switch (line_buf[col]) {
                case ' ':
                case '\t':
                    break;  // skip tab
                case '@':   // skip @
                case '`':
                case '$':
                    Message::print_message(Message::kWarning,
                                           "Unexpected symbol, ignoring.", line,
                                           std::make_pair(row, col));
                    break;
                case '/': {  // handling comment
                    if (line_buf[col + 1] == '/') {
                        goto NEXT_LINE;  // skip line
                    } else if (line_buf[col + 1] == '*') {
                        // a special case where multiple line comment is used as
                        // single line comment
                        if (std::string::npos == line.find("*/", col + 2)) {
                            multiple_line_comment = true;
                            multiple_line_comment_start = {row, col};
                            goto NEXT_LINE;
                        } else {
                            col = line.find("*/", col + 2) + 1;
                        }
                    } else {
                        goto HANDLE_SYMBOL;  // direct to handle symbol case
                    }
                } break;
                case '"': {  // handling string literal
                    auto start = col + 1;
                    col = line.find_first_of('\"', start);

                    // if not found other '"' until end of line
                    if (std::string::npos == col) {
                        have_error = true;
                        Message::print_message(Message::kError,
                                               "unclosed string literal", line,
                                               std::make_pair(row, start - 1));
                        col = line.length();
                    } else {
                        // handling escape sequence
                        while (col != std::string::npos &&
                               (line_buf[col - 1] == '\\' ||
                                line_buf[col] != '\"')) {
                            if (col - 2 >= 0 && line_buf[col - 2] == '\\') {
                                /*
                                 * Cases like ["This is evil \\"] where second
                                 * \ is already escaped by first \
                                 */
                                break;
                            }
                            col = line.find_first_of('\"', col + 1);
                        }

                        // if not found other '"' until end of line after escape
                        // sequence
                        if (std::string::npos == col) {
                            have_error = true;
                            Message::print_message(
                                Message::kError, "unclosed string literal",
                                line, std::make_pair(row, start - 1));
                            col = line.length();
                        }
                    }

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
                    tokens.emplace_back(tmp_string, TokenType::kString, row,
                                        start - 1, line);
                } break;
                case '\'': {  // handling char literal
                    auto start = col + 1;
                    col = line.find_first_of('\'', start);

                    // handling escape sequence
                    while (line_buf[col - 1] == '\\')
                        col = line.find_first_of('\'', col + 1);

                    tokens.emplace_back(line.substr(start, col - start),
                                        TokenType::kChar, row, start, line);
                } break;
                default: {
                    if (std::isdigit(line_buf[col])) {
                        auto start = col;
                        TokenType type = TokenType::kInteger;
                        col = line.find_first_not_of("0123456789", start);

                        if (col != std::string::npos) {
                            // skip hex literal and binary literal
                            if (std::tolower(line_buf[col]) == 'b' ||
                                std::tolower(line_buf[col]) == 'x') {
                                col++;
                            }

                            // if real number change type to real
                            if (line_buf[col] == '.') {
                                type = TokenType::kReal_number;
                                col++;
                            }
                        } else {
                            col = line.length();
                        }

                        // numbers are ended by a special Symbol or a space,
                        // correctness will be checked in syntax part3. Here we
                        // want to escape Dot(.) because it could be part3 of
                        // real number
                        while (col <= line.length() &&
                               (line_buf[col] == '.' ||
                                (!std::isspace(line_buf[col]) &&
                                 !SymbolUtils::IsOperator(line_buf[col])))) {
                            // we want to escape Minus(-) directly after [E]
                            // because it could be part3 of real number
                            // (e.g. 1.0E-2)
                            if (std::tolower(line_buf[col]) == 'e' &&
                                (line_buf[col + 1] == '-' ||
                                 line_buf[col + 1] == '+')) {
                                col++;
                                type = TokenType::kReal_number;
                            }
                            col++;
                        }

                        // do number format check
                        auto tmp_numbers = line.substr(start, col - start);
                        if (type == TokenType::kInteger) {
                            if (tmp_numbers[0] != '0' ||
                                (tmp_numbers[0] == '0' &&
                                 tmp_numbers.length() ==
                                     1)) {  // decimal number
                                // check format
                                if (!std::regex_match(tmp_numbers,
                                                      kInteger_number_regex)) {
                                    have_error = true;
                                    Message::print_message(
                                        Message::kError,
                                        "invalid integer number", line,
                                        std::make_pair(row, start));
                                }

                                // check range
                                try {
                                    if (std::stoll(tmp_numbers) >
                                        std::numeric_limits<int64_t>::max()) {
                                        have_error = true;
                                        Message::print_message(
                                            Message::kError,
                                            "integer number overflow", line,
                                            std::make_pair(row, start));
                                    }
                                } catch (const std::out_of_range& e) {
                                    have_error = true;
                                    Message::print_message(
                                        Message::kError,
                                        "integer number out of range", line,
                                        std::make_pair(row, start));
                                }

                            } else if (std::tolower(tmp_numbers[1]) == 'x') {
                                // check format
                                if (!std::regex_match(tmp_numbers,
                                                      kHex_number_regex)) {
                                    have_error = true;
                                    Message::print_message(
                                        Message::kError,
                                        "invalid hex number: [" + tmp_numbers +
                                            "]",
                                        line, std::make_pair(row, start));
                                }

                                // check range
                                try {
                                    if (std::stoll(tmp_numbers, nullptr, 16) >
                                        std::numeric_limits<int64_t>::max()) {
                                        have_error = true;
                                        Message::print_message(
                                            Message::kError,
                                            "hex number overflow", line,
                                            std::make_pair(row, start));
                                    }
                                } catch (const std::out_of_range& e) {
                                    have_error = true;
                                    Message::print_message(
                                        Message::kError,
                                        "hex number out of range", line,
                                        std::make_pair(row, start));
                                }

                            } else if (std::tolower(tmp_numbers[1]) ==
                                       'b') {  // binary number
                                // check format
                                if (!std::regex_match(tmp_numbers,
                                                      kBinary_number_regex)) {
                                    have_error = true;
                                    Message::print_message(
                                        Message::kError,
                                        "invalid binary number: [" +
                                            tmp_numbers + "]",
                                        line, std::make_pair(row, start));
                                }

                                // check range
                                try {
                                    if (std::stoll(tmp_numbers, nullptr, 2) >
                                        std::numeric_limits<int64_t>::max()) {
                                        have_error = true;
                                        Message::print_message(
                                            Message::kError,
                                            "binary number overflow", line,
                                            std::make_pair(row, start));
                                    }
                                } catch (const std::out_of_range& e) {
                                    have_error = true;
                                    Message::print_message(
                                        Message::kError,
                                        "binary number out of range", line,
                                        std::make_pair(row, start));
                                }

                            } else {  // octo number
                                // check format
                                if (!std::regex_match(tmp_numbers,
                                                      kOct_number_regex)) {
                                    have_error = true;
                                    Message::print_message(
                                        Message::kError, "invalid octo number",
                                        line, std::make_pair(row, start));
                                }

                                // check range
                                try {
                                    if (std::stoll(tmp_numbers, nullptr, 8) >
                                        std::numeric_limits<int64_t>::max()) {
                                        have_error = true;
                                        Message::print_message(
                                            Message::kError,
                                            "octo number overflow", line,
                                            std::make_pair(row, start));
                                    }
                                } catch (const std::out_of_range& e) {
                                    have_error = true;
                                    Message::print_message(
                                        Message::kError,
                                        "octo number out of range", line,
                                        std::make_pair(row, start));
                                }
                            }
                        } else {
                            // check real number format
                            if (!std::regex_match(tmp_numbers,
                                                  kReal_number_regex)) {
                                have_error = true;
                                Message::print_message(
                                    Message::kError,
                                    "invalid real number literal", line,
                                    std::make_pair(row, start));
                            }

                            // check real number range
                            try {
                                if (std::stod(tmp_numbers) >
                                        std::numeric_limits<
                                            long double>::max() ||
                                    std::stod(tmp_numbers) <
                                        std::numeric_limits<
                                            long double>::lowest()) {
                                    have_error = true;
                                    Message::print_message(
                                        Message::kError,
                                        "real number literal out of range",
                                        line, std::make_pair(row, start));
                                }
                            } catch (const std::out_of_range& e) {
                                have_error = true;
                                Message::print_message(
                                    Message::kError,
                                    "real number literal out of range", line,
                                    std::make_pair(row, start));
                            }

                            // check the significant digits
                            auto dot_pos = tmp_numbers.find('.');
                            if ((tmp_numbers.length() - dot_pos) >=
                                kMaxSignificantDigits) {
                                tmp_numbers.erase(
                                    dot_pos + kMaxSignificantDigits + 1,
                                    tmp_numbers.length());
                                Message::print_message(
                                    Message::kWarning,
                                    "real number is greater than most possible "
                                    "significant digits, will truncating to " +
                                        tmp_numbers,
                                    line, std::make_pair(row, start));
                            }
                        }

                        tokens.emplace_back(tmp_numbers, type, row, start,
                                            line);
                        col--;  // back to the last digit
                    } else if (TokenType::kUnknown ==
                               SymbolUtils::GetSymbolType(line_buf[col])) {
                        size_t start = col;

                        // find the next closest symbol
                        while (col != std::string::npos &&
                               col < line.length() &&
                               !SymbolUtils::IsOperator(line_buf[col]))
                            col++;

                        // handling if at line end
                        if (col == std::string::npos) {
                            col = line.length();
                        }

                        auto word = line.substr(start, col - start);
                        auto type = SymbolUtils::GetStringKeywordType(word);

                        if (type != TokenType::kUnknown) {  // splitting keyword
                            tokens.emplace_back(type, row, start, line);
                        } else if (SymbolUtils::IsPrimitiveType(
                                       word.c_str())) {  // splitting primitive
                                                         // type
                            tokens.emplace_back(word, TokenType::kType, row,
                                                start, line);
                        } else {  // identifier
                            tokens.emplace_back(word, TokenType::kIdentity, row,
                                                start, line);
                        }
                        col--;  // back to the last digit
                    } else {
                    HANDLE_SYMBOL:
                        auto type = SymbolUtils::GetSymbolType(
                            (const char*)(line_buf + col));

                        /**
                         * Here is not add the second symbol to token stream in
                         * this loop is because there is a special situation
                         * where the comment symbol is after the first symbol
                         * like this: [int a = 1; // comment] or [int a = 1; /\*
                         * start comment *\/] Which could cause parsing error,
                         * there is no need to insert special logic here since
                         * only redo one symbol would not influence the
                         * performance very much.
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

    // return error if you have error in the source code
    if (have_error) {
        return {Status::INTERNAL, "Parsing error"};
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Lexical
