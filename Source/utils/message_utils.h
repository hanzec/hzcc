//
// Created by chen_ on 2022/2/3.
//
#include <filesystem>

#include "lexical/Token.h"
#include "utils/logging.h"

#ifndef MYCC_PRINT_ERROR_MESSAGE_H
#define MYCC_PRINT_ERROR_MESSAGE_H
namespace Mycc::Message {
enum Level {
    kError,
    kFatalError,
    kWarning,
    kInfo,
};

void set_current_part(const std::string_view& part);

void set_current_file(const std::filesystem::path& filename);

void print_message(Level error_level, const std::string& message,
                   std::pair<int, int> line_info);

void print_message(Level error_level, const std::string& message,
                   const std::string& line, std::pair<int, int> line_info);

void print_message(Level error_level, const std::string& message,
                   const std::string& line, std::pair<int, int> line_info,
                   const std::string& error_string);

#define MYCC_PrintTokenError(token, message)                                 \
    DLOG(WARNING) << "Source file error found!";                             \
    Message::print_message(Message::kError, (message), (token).SourceLine(), \
                           (token).Location(), (token).Value());

#define MYCC_PrintFirstTokenError(tokens, message) \
    auto error_token = (tokens).front();           \
    MYCC_PrintTokenError(error_token, message)

#define MYCC_PrintTokenError_ReturnNull(token, message) \
    MYCC_PrintTokenError(token, message) return nullptr;

#define MYCC_PrintFirstTokenError_ReturnNull(tokens, message)      \
    auto error_token =                                             \
        (tokens).empty()                                           \
            ? Lexical::Token(Lexical::TokenType::kUnknown, -1, -1) \
            : (tokens).front();                                    \
    MYCC_PrintTokenError_ReturnNull(error_token, message)

}  // namespace Mycc::Message

#endif  // MYCC_PRINT_ERROR_MESSAGE_H
