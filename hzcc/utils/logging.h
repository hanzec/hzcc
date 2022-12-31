#include <glog/logging.h>

#include <iomanip>

#include "macro.h"
#include "parser/common/name_utils.h"
#ifndef HZCC_UTILS_LOGGING_H_
#define HZCC_UTILS_LOGGING_H_
namespace hzcc {
void initLogging(char argv[]);

#define VLOG_ERROR 0
#define CODE_GEN_LEVEL 1
#define OPT_LOG_LEVEL 2
#define AST_LOG_LEVEL 3
#define SYNTAX_LOG_LEVEL 4
#define LEXICAL_LOG_LEVEL 5
#define HZCC_VLOG_DEBUG_LEVEL 6

#define INTERNAL_LOG(level) LOG(level) << "\033[1;31m[Internal]\033[0m: "

#define INTERNAL_LOG_IF(level, cond) \
    LOG_IF(level, (cond)) << "\033[1;31m[Internal]\033[0m: "

#define HZCC_PRETTY_PRINT_TOKEN(token)                                        \
    std::setfill(' ') << "\033[0;33m[" << std::setw(8) << std::right          \
                      << ((int)(token).Type() <= 0xFF                         \
                              ? "Symbol"                                      \
                              : "" + ::hzcc::to_string((token).Type()))       \
                      << "]<" << std::setw(3) << (token).loc().first + 1 \
                      << "," << std::setw(3) << (token).loc().second     \
                      << ">" << std::setw(-1) << ":@ " << std::left           \
                      << std::setw(10) << (token).val(true) << " @\033[0m"  \
                      << std::setw(-1)

#define HZCC_INTERNAL_STATUS_MACROS_IMPL_CONCAT_NAME_INNER(x, y) x##y
#define HZCC_INTERNAL_STATUS_MACROS_IMPL_CONCAT_NAME(x, y) \
    HZCC_INTERNAL_STATUS_MACROS_IMPL_CONCAT_NAME_INNER(x, y)
#define HZCC_CHECK_OK_AND_ASSIGN_IMPL(status_name, lhs, rexpr)         \
    auto status_name = (rexpr);                                        \
    if (!status_name.ok()) {                                           \
        VLOG(HZCC_VLOG_DEBUG_LEVEL)                                    \
            << "Passing Error [" << std::hex << (void*)&(*status_name) \
            << "] to upper layer";                                     \
        return status_name.status();                                   \
    }                                                                  \
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

enum PACKED CompileErrorLevel {
    kCompileErrorLevel_Error = 0,
    kCompileErrorLevel_Warning = 1,
    kCompileErrorLevel_Info = 2,
    kCompileErrorLevel_SIZE,
};


namespace message {

void setFilename(const std::string& filename);

void print_message(CompileErrorLevel level,         // NOLINT
                   std::string_view errorMessage);  // NOLINT

void print_message(CompileErrorLevel level,             // NOLINT
                   std::string_view errorMessage,       // NOLINT
                   std::pair<uint64_t, uint64_t> loc);  // NOLINT

const std::string& getFilename();

}  // namespace message
}  // namespace hzcc
#endif  // HZCC_UTILS_LOGGING_H_
