//
// Created by chen_ on 2022/2/3.
//

#include "message_utils.h"

#include <glog/logging.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "fs_utils.h"
#include "macro.h"
#include "options.h"

namespace Hzcc::Message {
static std::filesystem::path current_filename = "NO_FILE_NAME_AVALIABLE";

constexpr const char* KEnableRed = "\033[1;31m";
constexpr const char* KEnableGreen = "\033[1;32m";
constexpr const char* KEnableYellow = "\033[1;33m";
constexpr const char* KDisableColor = "\033[0m";

std::string get_current_file() { return current_filename.string(); }

static void ALWAYS_INLINE internal_print_nice_message(
    Level error_level, const std::string& message, const std::string& line,
    std::pair<int, int> line_info) {
    // print colored message header
    // we add 1 to line_info.first because in code we count from 0.
    std::cout << current_filename.c_str() << ":" << (line_info.first + 1) << ":"
              << line_info.second << ": ";

    if (Options::Global_disable_color) {
        if (error_level == Level::kError) {
            std::cout << "error: ";
        } else if (error_level == Level::kWarning) {
            std::cout << "warning: ";
        } else {
            std::cout << "info: ";
        }
    } else {
        if (error_level == Level::kError) {
            std::cout << KEnableRed << "error: " << KDisableColor;
        } else if (error_level == Level::kWarning) {
            std::cout << KEnableYellow << "warning: " << KDisableColor;
        } else {
            std::cout << "info: ";
        }
    }

    std::cout << message << std::endl
              << "  " << std::setw(5) << line_info.first << std::setw(0)
              << " | ";

    // print source code line
    for (int i = 0; i < line.length(); ++i) {
        if (line_info.second == i && !Options::Global_disable_color) {
            if (error_level == Level::kError) {
                std::cout << KEnableRed;
            } else if (error_level == Level::kWarning) {
                std::cout << KEnableYellow;
            }
            std::cout << line[i] << KEnableRed;
        } else {
            std::cout << line[i];
        }
    }

    if (!Options::Global_disable_color) {
        std::cout << KDisableColor;
    }
    std::cout << std::endl;

    // print narrow point to error position
    std::cout << std::string(line_info.second + 10, ' ') << "^" << std::endl
              << std::flush;
}

void set_current_file(const std::filesystem::path& filename) {
    current_filename = filename;
}

void print_message_internal(Level error_level, const std::string& message,
                            const std::string& line,
                            std::pair<int, int> line_info) {
    print_message_internal(
        error_level, message, line, line_info,
        std::string() + (line.empty() ? ' ' : line.at(line_info.second)));
}

void print_message_internal(Level error_level, const std::string& message,
                            std::pair<int, int> line_info) {
    if (FsUtils::is_readable(current_filename)) {
        std::fstream file(current_filename);
        file.seekg(std::ios::beg);
        for (int i = 0; i < line_info.first; i++) {
            file.ignore(std::numeric_limits<std::streamsize>::max(),
                        file.widen('\n'));
        }
        std::string line;
        std::getline(file, line);
        internal_print_nice_message(error_level, message, line, line_info);
    } else {
        LOG(ERROR) << "File: " << current_filename << " is not readable";
        internal_print_nice_message(error_level, message, "", line_info);
    }
}

void print_message_internal(Level error_level, const std::string& message,
                            const std::string& line,
                            std::pair<int, int> line_info,
                            const std::string& error_string) {
    internal_print_nice_message(error_level, message, line, line_info);
}

}  // namespace Hzcc::Message
