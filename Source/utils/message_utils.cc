//
// Created by chen_ on 2022/2/3.
//

#include "message_utils.h"

#include <glog/logging.h>

#include <fstream>
#include <iostream>

#include "fs_utils.h"
#include "options.h"
namespace Mycc::Message {

constexpr const char* KEnableRed = "\033[1;31m";
constexpr const char* KEnableGreen = "\033[1;32m";
constexpr const char* KEnableYellow = "\033[1;33m";
constexpr const char* KDisableColor = "\033[0m";

static void internal_print_nice_message(Level error_level, const std::string& message,
                                        const std::string& file_name, const std::string& line,
                                        std::pair<int, int> line_info) {
  // print colored message header
  std::cerr << file_name << ":" << line_info.first << ":" << line_info.second << ": ";

  if (Options::Global_disable_color) {
    if (error_level == Level::kError) {
      std::cerr << "error: ";
    } else if (error_level == Level::kWarning) {
      std::cerr << "warning: ";
    } else {
      std::cerr << "info: ";
    }
  } else {
    if (error_level == Level::kError) {
      std::cerr << KEnableRed << "error: " << KDisableColor;
    } else if (error_level == Level::kWarning) {
      std::cerr << KEnableYellow << "warning: " << KDisableColor;
    } else {
      std::cerr << "info: ";
    }
  }

  std::cerr << message << std::endl
            << "  " << std::setw(5) << line_info.first << std::setw(0) << "|";

  // print source code line
  for (int i = 0; i < line.length(); ++i) {
    if (line_info.second == i && !Options::Global_disable_color) {
      if (error_level == Level::kError) {
        std::cerr << KEnableRed;
      } else if (error_level == Level::kWarning) {
        std::cerr << KEnableYellow;
      }
      std::cerr << line[i] << KEnableRed;
    } else {
      std::cerr << line[i];
    }
  }

  if (!Options::Global_disable_color) {
    std::cerr << KDisableColor;
  }
  std::cerr << std::endl;

  // print narrow point to error position
  std::cerr << "\t" << std::string(line_info.second + 7, ' ') << "^" << std::endl
            << std::flush;
}

void print_message(Level error_level, const std::string& message,
                   const std::filesystem::path& file_path, std::pair<int, int> line_info) {
  if (FsUtils::is_readable(file_path)) {
    std::fstream file(file_path);
    file.seekg(std::ios::beg);
    for (int i = 0; i < line_info.first; i++) {
      file.ignore(std::numeric_limits<std::streamsize>::max(), file.widen('\n'));
    }
    std::string line;
    std::getline(file, line);
    print_message(error_level, message, line, file_path, line_info);
  } else {
    LOG(ERROR) << "File: " << file_path << " is not readable";
    print_message(error_level, message, "", file_path, line_info);
  }
}

void print_message(Level error_level, const std::string& message, const std::string& line,
                   const std::string& file_path, std::pair<int, int> line_info) {
  if (Options::Global_enable_nicer_print) {
    internal_print_nice_message(error_level, message, file_path, line, line_info);
  } else {
    std::cerr << "Lexer error in file " << file_path << " line " << line_info.first;
    if (line_info.second >= 0) {
      std::cerr << " near text: " << line[line_info.second] << std::endl
                << '\t' << message << std::endl
                << std::flush;
    }
  }
}

}  // namespace Mycc::Message