//
// Created by chen_ on 2022/2/3.
//
#ifndef MYCC_PRINT_ERROR_MESSAGE_H
#define MYCC_PRINT_ERROR_MESSAGE_H
#include <filesystem>

#include "lexical/Token.h"
#include "macro.h"
#include "utils/logging.h"

namespace Mycc::Message {
enum Level {
    kError,
    kFatalError,
    kWarning,
    kInfo,
};

void set_current_part(const std::string_view& part);

void set_current_file(const std::filesystem::path& filename);

void print_message_internal(Level error_level, const std::string& message,
                            std::pair<int, int> line_info);

void print_message_internal(Level error_level, const std::string& message,
                            const std::string& line,
                            std::pair<int, int> line_info);

void print_message_internal(Level error_level, const std::string& message,
                            const std::string& line,
                            std::pair<int, int> line_info,
                            const std::string& error_string);

void ALWAYS_INLINE print_message(Level error_level, const std::string& message,
                                 std::pair<int, int> line_info) {
    print_message_internal(error_level, message, line_info);
}

void ALWAYS_INLINE print_message(Level error_level, const std::string& message,
                                 const std::string& line,
                                 std::pair<int, int> line_info) {
    print_message_internal(error_level, message, line, line_info);
}

void ALWAYS_INLINE print_message(Level error_level, const std::string& message,
                                 const std::string& line,
                                 std::pair<int, int> line_info,
                                 const std::string& error_string) {
    print_message_internal(error_level, message, line, line_info, error_string);
}

#ifndef NDEBUG
class PrintMessageReporter {
  public:
    PrintMessageReporter(std::string Caller, std::string File, int Line)
        : caller_(std::move(Caller)), file_(std::move(File)), line_(Line) {}

    void operator()(Level error_level, const std::string& message,
                    std::pair<int, int> line_info) {
        DLOG(ERROR) << "Error raised by [" << caller_ << "] "
                                 << file_ << ":" << line_ << std::endl;
        return print_message(error_level, message, line_info);
    }

    void operator()(Level error_level, const std::string& message,
                    const std::string& line, std::pair<int, int> line_info) {
        DLOG(ERROR) << "Error raised by [" << caller_ << "] "
                                 << file_ << ":" << line_ << std::endl;
        return print_message(error_level, message, line, line_info);
    }

    void operator()(Level error_level, const std::string& message,
                    const std::string& line, std::pair<int, int> line_info,
                    const std::string& error_string) {
        DLOG(ERROR) << "Error raised by [" << caller_ << "] "
                                 << file_ << ":" << line_ << std::endl;
        return print_message(error_level, message, line, line_info,
                             error_string);
    }

  private:
    int line_;
    std::string file_;
    std::string caller_;
};

#undef print_message
#define print_message PrintMessageReporter(__FUNCTION__, __FILE__, __LINE__)
#endif

#define MYCC_PrintTokenError(token, message)                                 \
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
