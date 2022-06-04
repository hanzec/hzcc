//
// Created by chen_ on 2022/2/3.
//
#ifndef HZCC_PRINT_ERROR_MESSAGE_H
#define HZCC_PRINT_ERROR_MESSAGE_H
#include <filesystem>

#include "lexical/Token.h"
#include "macro.h"
#include "options.h"
#include "utils/logging.h"

namespace Hzcc::Message {
enum Level {
    kError,
    kFatalError,
    kWarning,
    kInfo,
};

void set_current_part(const std::string_view& part);

std::string get_current_file();

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

#ifndef NDEBUG
class PrintMessageReporter {
  public:
    PrintMessageReporter(std::string Caller, std::string File, int Line)
        : caller_(std::move(Caller)),
          _input_file(std::move(File)),
          line_(Line) {}

    void operator()(Level error_level, const std::string& message,
                    std::pair<int, int> line_info) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "Error raised by [" << caller_ << "] " << _input_file << ":"
            << line_ << std::endl;
        return print_message_internal(error_level, message, line_info);
    }

    void operator()(Level error_level, const std::string& message,
                    const std::string& line, std::pair<int, int> line_info) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "Error raised by [" << caller_ << "] " << _input_file << ":"
            << line_ << std::endl;
        return print_message_internal(error_level, message, line, line_info);
    }

    void operator()(Level error_level, const std::string& message,
                    const std::string& line, std::pair<int, int> line_info,
                    const std::string& error_string) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "Error raised by [" << caller_ << "] " << _input_file << ":"
            << line_ << std::endl;
        return print_message_internal(error_level, message, line, line_info,
                                      error_string);
    }

  private:
    int line_;
    std::string _input_file;
    std::string caller_;
};

#undef print_message
#define print_message PrintMessageReporter(__FUNCTION__, __FILE__, __LINE__)
#else
#undef print_message
#define print_message print_message_internal
#endif
}  // namespace Hzcc::Message

#endif  // HZCC_PRINT_ERROR_MESSAGE_H
