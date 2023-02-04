//
// Created by chen_ on 2022/3/26.
//

#include "utils/logging.h"

#include <glog/log_severity.h>

#include <array>
#include <fstream>  // IWYU pragma: keep
#include <iomanip>
#include <iostream>
#include <limits>
#include <magic_enum.hpp>

#include "fs_utils.h"
#include "options.h"

namespace hzcc {

namespace {
constexpr const char* KEnableRed = "\033[1;31m";
constexpr const char* KEnableGreen = "\033[1;32m";
constexpr const char* KEnableYellow = "\033[1;33m";
constexpr const char* KDisableColor = "\033[0m";
}  // namespace

void custom_prefix_callback(std::ostream& out_stream,  // NOLINT
                            const google::LogMessageInfo& info, void*) {
    // print date with format: YYYY-MM-DD HH:MM:SS
    out_stream << std::put_time(std::localtime(&info.time.timestamp()), "%F %T")
               << " ";

    // print thread id
    out_stream << std::setw(5) << std::left << info.thread_id << " ";

    // print color log
    if (!Options::Global_disable_color) {
        switch (info.severity[0]) {
            case 'I':
                out_stream << KEnableGreen;
                break;
            case 'W':
                out_stream << KEnableYellow;
                break;
            case 'E':
            case 'F':
                out_stream << KEnableRed;
                break;
            default:
                break;
        }
    }

    out_stream << "[" << info.severity[0] << info.severity[1]
               << info.severity[2] << info.severity[3] << "] " << std::setw(0)
               << std::right;

    if (!Options::Global_disable_color) {
        out_stream << KDisableColor;
    }

    // print file name and line number
    out_stream << info.filename << ":" << info.line_number << "]";
}

void initLogging(char* argv[]) {
    google::InitGoogleLogging(argv[0], &custom_prefix_callback);
    google::SetStderrLogging(google::GLOG_INFO);
}

namespace message {
static std::string CURRENT_FILE_NAME;

void set_current_file_name(const std::string& file_name) {
    CURRENT_FILE_NAME = file_name;
}

void print_message(CompileErrorLevel level,              // NOLINT
                   std::string_view errorMessage,        // NOLINT
                   std::pair<uint64_t, uint64_t> loc) {  // NOLINT
    static constexpr std::array<const char*, 3> kLevelColorTable = {
        KEnableRed, KEnableYellow, KEnableGreen};
    static constexpr std::array<const char*, 3> kLevelStringTable = {
        "error: ", "warning: ", "info: "};

    if (FsUtils::is_readable(CURRENT_FILE_NAME)) {
        std::fstream file(CURRENT_FILE_NAME);
        file.seekg(std::ios::beg);
        for (int i = 0; i < loc.first; i++) {
            file.ignore(std::numeric_limits<std::streamsize>::max(),
                        file.widen('\n'));
        }
        std::string line;
        std::getline(file, line);

        // print colored message header
        // we add 1 to line_info.first because in code we count from 0.
        std::cerr << CURRENT_FILE_NAME << ":" << loc.first << ":" << loc.second
                  << ": ";

        if (Options::Global_disable_color) {
            std::cerr << kLevelStringTable[magic_enum::enum_integer(level)];
        } else {
            std::cerr << kLevelColorTable[magic_enum::enum_integer(level)]
                      << kLevelStringTable[magic_enum::enum_integer(level)]
                      << KDisableColor;
        }

        std::cerr << errorMessage << std::endl
                  << "  " << std::setw(5) << loc.first << std::setw(0) << " | ";

        // print source code line
        for (int i = 0; i < line.length(); ++i) {
            if (loc.second == i && !Options::Global_disable_color) {
                std::cerr << kLevelColorTable[magic_enum::enum_integer(level)]
                          << line[i];
            } else {
                std::cerr << line[i];
            }
        }

        if (!Options::Global_disable_color) {
            std::cerr << KDisableColor;
        }
        std::cerr << std::endl;

        // print narrow point to error position
        std::cerr << std::string(loc.second + 10, ' ') << "^" << std::endl
                  << std::flush;

    } else {
        LOG(FATAL) << "file " + CURRENT_FILE_NAME + " is not readable";
    }
}
}  // namespace message
}  // namespace hzcc
