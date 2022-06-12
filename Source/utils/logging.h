#include <glog/logging.h>

#include <iomanip>

#include "lexical/utils/symbol_utils.h"
#ifndef HZCC_UTILS_LOGGING_H_
#define HZCC_UTILS_LOGGING_H_
namespace Hzcc {
void initLogging(char argv[]);

#define VLOG_ERROR 0
#define CODE_GEN_LEVEL 1
#define OPT_LOG_LEVEL 2
#define AST_LOG_LEVEL 3
#define SYNTAX_LOG_LEVEL 4
#define LEXICAL_LOG_LEVEL 5
#define MESSAGE_ERROR_TRACING 6

#define HZCC_RUNTIME_CHECK(cond)                                    \
    static_assert(std::is_convertible<decltype(cond), bool>::value, \
                  "Macro type mismatch, need bool for cond");       \
    LOG_IF(FATAL, !(cond)) << "\033[1;31m[Internal]\033[0m: "

#define HZCC_PRETTY_PRINT_TOKEN(token)                                        \
    std::setfill(' ') << "\033[0;33m[" << std::setw(8) << std::right          \
                      << ((int)(token).Type() <= 0xFF                         \
                              ? "Symbol"                                      \
                              : "" + Lexical::SymbolUtils::TokenTypeToString( \
                                         (token).Type()))                     \
                      << "]<" << std::setw(3) << (token).Location().first + 1 \
                      << "," << std::setw(3) << (token).Location().second     \
                      << ">" << std::setw(-1) << ":@ " << std::left           \
                      << std::setw(10) << (token).Value(true) << " @\033[0m"  \
                      << std::setw(-1)
}  // namespace Hzcc
#endif  // HZCC_UTILS_LOGGING_H_
