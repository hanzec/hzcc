//
// Created by chen_ on 2022/2/3.
//

#ifndef MYCC_PRINT_ERROR_MESSAGE_H
#define MYCC_PRINT_ERROR_MESSAGE_H
#include <filesystem>
namespace Mycc::Message {
enum Level {
  kError,
  kFatalError,
  kWarning,
  kInfo,
};

void print_message(Level error_level, const std::string& message,
                   const std::filesystem::path& file_path, std::pair<int, int> line_info);

void print_message(Level error_level, const std::string& message, const std::string& line,
                   const std::string& file_path, std::pair<int, int> line_info);

}  // namespace Mycc::Message

#endif  // MYCC_PRINT_ERROR_MESSAGE_H
