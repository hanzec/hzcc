//
// Created by chen_ on 2022/3/26.
//

#include "utils/logging.h"

#include <glog/log_severity.h>
#include <array>
#include <iomanip>
#include <iostream>
#include <limits>
#include <fstream> // IWYU pragma: keep

#include "fs_utils.h"
#include "options.h"

namespace hzcc {
void initLogging(char argv[]) {
    google::InitGoogleLogging(argv);
    google::SetStderrLogging(google::GLOG_INFO);
}

namespace message {
static std::string CURRENT_FILE_NAME;

static constexpr const char* KEnableRed = "\033[1;31m";
static constexpr const char* KEnableGreen = "\033[1;32m";
static constexpr const char* KEnableYellow = "\033[1;33m";
static constexpr const char* KDisableColor = "\033[0m";

void set_current_file_name(const std::string& file_name) {
    CURRENT_FILE_NAME = file_name;
}

void print_message(CompileErrorLevel level,               // NOLINT
                   const std::string& errorMessage,       // NOLINT
                   std::pair<uint64_t, uint64_t>& loc) {  // NOLINT
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
            std::cerr << kLevelStringTable[level];
        } else {
            std::cerr << kLevelColorTable[level] << kLevelStringTable[level]
                      << KDisableColor;
        }

        std::cerr << errorMessage << std::endl
                  << "  " << std::setw(5) << loc.first << std::setw(0) << " | ";

        // print source code line
        for (int i = 0; i < line.length(); ++i) {
            if (loc.second == i && !Options::Global_disable_color) {
                std::cerr << kLevelColorTable[level] << line[i];
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
        INTERNAL_LOG(FATAL) << "file " + CURRENT_FILE_NAME + " is not readable";
    }
}
}  // namespace message
}  // namespace hzcc
