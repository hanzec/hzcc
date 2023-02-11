#include <glog/logging.h>

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "macro.h"
#ifndef HZCC_UTILS_LOGGING_H_
#define HZCC_UTILS_LOGGING_H_
namespace hzcc {
void initLogging(char* argv[]);

/**
 * ------------------------------------------------------------------------
 * ### LOG Print Macros                                                 ###
 * ------------------------------------------------------------------------
 */

#define VLOG_ERROR 0
#define CODE_GEN_LEVEL 1
#define OPT_LOG_LEVEL 2
constexpr int AST_LOG = 3;
constexpr int SYNTAX_LOG = 4;
constexpr int LEXICAL_LOG = 5;
constexpr int DEBUG_INFO = 6;

/**
 * ------------------------------------------------------------------------
 * ### Debug Print Macros                                               ###
 * ------------------------------------------------------------------------
 */
#define HZCC_PRETTY_PRINT_TOKEN(token)                                        \
    std::setfill(' ') << "\033[0;33m[" << std::setw(8) << std::right          \
                      << ((int)(token).type() <= 0xFF                         \
                              ? "Symbol"                                      \
                              : magic_enum::enum_name((token).type()))        \
                      << "]<" << std::setw(3) << (token).loc().first + 1      \
                      << "," << std::setw(3) << (token).loc().second << ">"   \
                      << std::setw(-1) << ":@ " << std::left << std::setw(10) \
                      << (token).to_str(true) << " @\033[0m" << std::setw(-1)

/**
 * ------------------------------------------------------------------------
 * ### Status Check Macros                                              ###
 * ------------------------------------------------------------------------
 */
#define HZCC_INTERNAL_STATUS_MACROS_IMPL_CONCAT_NAME_INNER(x, y) x##y
#define HZCC_INTERNAL_STATUS_MACROS_IMPL_CONCAT_NAME(x, y) \
    HZCC_INTERNAL_STATUS_MACROS_IMPL_CONCAT_NAME_INNER(x, y)
#define HZCC_CHECK_OK_AND_ASSIGN_IMPL(status_name, lhs, rexpr)               \
    auto status_name = (rexpr);                                              \
    if (!status_name.ok()) {                                                 \
        LOG(INFO) << "Passing Error [" << std::hex << (void*)&(*status_name) \
                  << "] to upper layer";                                     \
        return status_name.status();                                         \
    }                                                                        \
    auto lhs = std::move(status_name).value();

#define HZCC_CHECK_OR_ASSIGN(lhs, rexpr)                               \
    HZCC_CHECK_OK_AND_ASSIGN_IMPL(                                     \
        HZCC_INTERNAL_STATUS_MACROS_IMPL_CONCAT_NAME(_status_or_value, \
                                                     __COUNTER__),     \
        lhs, rexpr)

#ifdef HZCC_ENABLE_RUNTIME_CHECK
#define HZCC_RUNTIME_CHECK_BLOCK(cond) \
    { cond }
#else
#define HZCC_RUNTIME_CHECK_BLOCK(cond)
#endif

/**
 * ------------------------------------------------------------------------
 * ### Logging Functions                                                ###
 * ------------------------------------------------------------------------
 */
enum class CompileErrorLevel {
    Error = 0,
    Warning = 1,
    Info = 2,
};

namespace message {

void set_current_file_name(const std::string& filename);

void print_message(CompileErrorLevel level,             // NOLINT
                   std::string_view errorMessage,       // NOLINT
                   std::pair<uint64_t, uint64_t> loc);  // NOLINT

const std::string& getFilename();

}  // namespace message
}  // namespace hzcc
#endif  // HZCC_UTILS_LOGGING_H_
