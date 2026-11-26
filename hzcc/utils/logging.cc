//
// Created by chen_ on 2022/3/26.
//

#include "utils/logging.h"

#include <glog/log_severity.h>

#include <algorithm>
#include <array>
#include <fstream>  // IWYU pragma: keep
#include <iomanip>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

#include "options.h"

namespace hzcc {

namespace {
constexpr const char* KEnableRed = "\033[1;31m";
constexpr const char* KEnableGreen = "\033[1;32m";
constexpr const char* KEnableYellow = "\033[1;33m";
constexpr const char* KDisableColor = "\033[0m";
constexpr std::array<const char*, 3> kLevelColorTable = {
    KEnableRed, KEnableYellow, KEnableGreen};
}  // namespace

/** @brief Write HZCC's timestamp, severity, thread, and source-location log prefix.
 * @param out_stream Destination provided by glog.
 * @param info Log-message metadata used to format the prefix.
 */
void custom_prefix_callback(std::ostream& out_stream,  // NOLINT
                            const google::LogMessage& info, void*) {
    // print date with format: YYYY-MM-DD HH:MM:SS
    const auto& time = info.time();
    out_stream << std::setfill('0') << std::setw(4) << 1900 + time.year() << "-"
               << std::setw(2) << 1 + time.month() << "-" << std::setw(2) << time.day()
               << " " << std::setw(2) << time.hour() << ":" << std::setw(2) << time.min()
               << ":" << std::setw(2) << time.sec() << std::setfill(' ') << " ";
    const char* severity = google::GetLogSeverityName(info.severity());

    // print thread id
    out_stream << std::setw(5) << std::left << info.thread_id() << " ";

    // print color log
    if (!Options::Global_disable_color) {
        switch (severity[0]) {
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

    out_stream << "[" << severity[0] << severity[1] << severity[2]
               << severity[3] << "] " << std::setw(0) << std::right;

    if (!Options::Global_disable_color) {
        out_stream << KDisableColor;
    }

    // print file name and line number
    out_stream << info.basename() << ":" << info.line() << "]";
}

void initLogging(char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    // glog 0.7 installs prefix callbacks separately from logger initialization.
    google::InstallPrefixFormatter(&custom_prefix_callback);
    google::SetStderrLogging(google::GLOG_INFO);
}

namespace message {
/** @brief Access the current thread's diagnostic source name.
 * @return Modifiable filename owned by this thread for its lifetime.
 */
static std::string& current_file_name() {
    static thread_local std::string file_name;
    return file_name;
}

void set_current_file_name(const std::string& file_name) {
    current_file_name() = file_name;
}

const std::string& getFilename() { return current_file_name(); }

void print_message(CompileErrorLevel level,              // NOLINT
                   std::string_view errorMessage,        // NOLINT
                   std::pair<uint64_t, uint64_t> loc,    // NOLINT
                   std::optional<std::string_view> source_line) {
    static constexpr std::array<const char*, 3> kLevelStringTable = {
        "error: ", "warning: ", "info: "};

    const auto& file_name = getFilename();
    const auto severity = magic_enum::enum_integer(level);

    // print colored message header
    std::cerr << (file_name.empty() ? "<input>" : file_name) << ":"
              << loc.first << ":" << loc.second << ": ";
    if (!Options::Global_disable_color) std::cerr << kLevelColorTable[severity];
    std::cerr << kLevelStringTable[severity];
    if (!Options::Global_disable_color) std::cerr << KDisableColor;
    std::cerr << errorMessage << '\n';

    print_source_context(loc, source_line, level);
}

void print_source_context(std::pair<uint64_t, uint64_t> loc,
                          std::optional<std::string_view> source_line,
                          CompileErrorLevel level) {
    const auto& file_name = getFilename();
    const auto severity = magic_enum::enum_integer(level);
    // Parser errors can refer to an earlier line. Lexical errors already have the
    // exact source text, including when the stream has no corresponding file.
    std::string saved_line;
    if (!source_line) {
        if (loc.first == 0 || file_name.empty()) return;
        std::ifstream input{file_name};
        for (uint64_t row = 1; row <= loc.first; ++row) {
            if (!std::getline(input, saved_line)) return;
        }
        source_line = saved_line;
    }
    if (source_line->empty()) return;

    // print source code line
    std::cerr << "  " << std::setw(5) << loc.first << " | ";
    const auto column = std::min<uint64_t>(loc.second, source_line->size());
    std::cerr << source_line->substr(0, column);
    if (!Options::Global_disable_color) std::cerr << kLevelColorTable[severity];
    std::cerr << source_line->substr(column);
    if (!Options::Global_disable_color) std::cerr << KDisableColor;

    // print narrow point to error position
    std::cerr << '\n' << "          ";
    for (char ch : source_line->substr(0, column)) {
        std::cerr << (ch == '\t' ? '\t' : ' ');
    }
    std::cerr << "^\n";
}
}  // namespace message
}  // namespace hzcc
