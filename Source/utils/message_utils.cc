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

static std::string_view current_part = "NO_INFO";

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
    std::cerr << current_filename.c_str() << ":" << (line_info.first + 1) << ":"
              << line_info.second << ": ";

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
    std::cerr << "\t" << std::string(line_info.second + 7, ' ') << "^"
              << std::endl
              << std::flush;
}

void set_current_part(const std::string_view& part) { current_part = part; }

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
        print_message(error_level, message, line, line_info);
    } else {
        LOG(ERROR) << "File: " << current_filename << " is not readable";
        print_message(error_level, message, "", line_info);
    }
}

void print_message_internal(Level error_level, const std::string& message,
                            const std::string& line,
                            std::pair<int, int> line_info,
                            const std::string& error_string) {
    if (Options::Global_enable_nicer_print) {
        internal_print_nice_message(error_level, message, line, line_info);
    } else {
        if (line_info.first == -1) {
            std::ifstream file(current_filename);
            line_info.first =
                std::count(std::istreambuf_iterator<char>(file),
                           std::istreambuf_iterator<char>(), '\n') +
                1;
        }

        // we add 1 to line_info.first because in code we count from 0.
        std::cerr << current_part << " error in file "
                  << current_filename.c_str() << " line "
                  << (line_info.first + 1) << " near "
                  << (line_info.second == -1
                          ? "end of file"
                          : (line_info.second == -2 ? ""
                                                    : "text " + error_string))
                  << "\n\t"
                  << message.substr(message.find_first_not_of(' '),
                                    message.find_last_not_of(' ') + 1)
                  << std::endl
                  << std::flush;
    }
}

}  // namespace Hzcc::Message
