#ifndef MYCC_UTILS_LOGGING_H_
#define MYCC_UTILS_LOGGING_H_
#include <glog/logging.h>
namespace Mycc {
void initLogging(char argv[]);

#define LEXICAL_LOG(level) \
    LOG(level) << "[" << __FILE__ << ":" << __LINE__ << "] "

#define VLOG_ERROR 0
#define CODE_GEN_LEVEL 1
#define OPT_LOG_LEVEL 2
#define AST_LOG_LEVEL 3
#define SYNTAX_LOG_LEVEL 4
#define LEXICAL_LOG_LEVEL 5
#define MESSAGE_ERROR_TRACING 6

#define DVLOG_IF(level, cond) (cond) ? (void)0 : DVLOG(level)

#define MYCC_PRETTY_PRINT_TOKEN(token)                                        \
    std::setfill(' ') << "\033[0;33m[" << std::setw(8) << std::right          \
                      << ((int)(token).Type() <= 0xFF                         \
                              ? "Symbol"                                      \
                              : "" + Lexical::SymbolUtils::TokenTypeToString( \
                                         (token).Type()))                     \
                      << "]<" << std::setw(3) << (token).Location().first + 1 \
                      << "," << std::setw(3) << (token).Location().second     \
                      << ">" << std::setw(-1) << ":@" << std::setw(10)        \
                      << (token).Value(true) << "@\033[0m" << std::setw(-1)
}  // namespace Mycc
#endif  // MYCC_UTILS_LOGGING_H_
