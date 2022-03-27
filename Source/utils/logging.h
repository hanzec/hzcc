#ifndef MYCC_UTILS_LOGGING_H_
#define MYCC_UTILS_LOGGING_H_
#include <glog/logging.h>
namespace Mycc {
void initLogging(char argv[]);

#define LEXICAL_LOG(level) \
    LOG(level) << "[" << __FILE__ << ":" << __LINE__ << "] "

#define AST_LOG_LEVEL 0
#define SYNTAX_LOG_LEVEL 1
#define LEXICAL_LOG_LEVEL 2

#define MYCC_PRETTY_PRINT_TOKEN(token)                                        \
    std::setfill(' ') << "\033[0;33m[" << std::setw(8) << std::right          \
                      << ((int)(token).Type() <= 0xFF                         \
                              ? "Symbol"                                      \
                              : "" + Lexical::SymbolUtils::TokenTypeToString( \
                                         (token).Type()))                     \
                      << "]<" << std::setw(3) << (token).Location().first     \
                      << "," << std::setw(3) << (token).Location().second     \
                      << ">" << std::setw(-1) << ":@" << std::setw(10)        \
                      << (token).Value(true) << "@\033[0m" << std::setw(-1)
}  // namespace Mycc
#endif  // MYCC_UTILS_LOGGING_H_
