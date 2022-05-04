//
// Created by chen_ on 2022/3/24.
//
#include <optional>
#include <utility>

#include "lexical/Token.h"
#include "lexical/utils/token_utils.h"
#include "macro.h"

#ifndef NDEBUG
#include <iomanip>

#include "lexical/utils/symbol_utils.h"
#include "utils/logging.h"
#endif
#ifndef MYCC_SOURCE_SYNTAX_UTILS_TOKEN_LIST_UTILS_H_
#define MYCC_SOURCE_SYNTAX_UTILS_TOKEN_LIST_UTILS_H_
namespace Hzcc::Syntax {
using TokenList = std::list<Lexical::Token>;

namespace TokenListUtils {
ALWAYS_INLINE std::optional<Lexical::Token> checkDuplicate(
    const std::list<Lexical::Token> &list) {
    for (auto it = list.begin(); it != list.end(); it++) {
        for (auto it2 = it; it2 != list.end(); it2++) {
            if (it->Type() == it2->Type() &&
                it->Location() != it2->Location()) {
                return *it2;
            }
        }
    }
    return std::nullopt;
}

ALWAYS_INLINE Lexical::Token pop_list(TokenList &list) {
    auto token = list.front();
    list.pop_front();
    return token;
}

ALWAYS_INLINE Lexical::Token peek(TokenList &list) { return list.front(); }

ALWAYS_INLINE Lexical::Token peek2(TokenList &list) {
    return *(std::next(list.begin()));
}

ALWAYS_INLINE Lexical::Token peekNExceptAttr(TokenList &list, int n) {
    int meet = 1;
    for (auto &it : list) {
        if (Lexical::TokenUtils::IsAttribute(it.Type())) {
            continue;
        }
        if (meet == n) {
            return it;
        }
        meet++;
    }
    return {Lexical::TokenType::END, -1, 1};
}

ALWAYS_INLINE Lexical::Token peek3(TokenList &list) {
    return *(std::next(std::next(list.begin())));
}

ALWAYS_INLINE TokenList ConcatAttribute(TokenList &attrs, TokenList &tokens) {
    while (!tokens.empty() &&
           Lexical::TokenUtils::IsAttribute(peek(tokens).Type())) {
        attrs.push_back(tokens.front());
        pop_list(tokens);
    }
    return attrs;
}

ALWAYS_INLINE TokenList GetAttribute(TokenList &tokens) {
    TokenList attribute;
    return ConcatAttribute(attribute, tokens);
}

ALWAYS_INLINE std::list<std::string> ConsumeTokenListToStr(TokenList &tokens) {
    std::list<std::string> result;
    while (!tokens.empty()) {
        result.push_back(pop_list(tokens).Value());
    }
    return result;
}

#ifndef NDEBUG
// class to capture the caller and print it.
class PopFuncReporter {
  public:
    PopFuncReporter(std::string Caller, std::string File, int Line)
        : caller_(std::move(Caller)), _input_file(std::move(File)), line_(Line) {}

    Lexical::Token operator()(TokenList &list) {
        DVLOG(SYNTAX_LOG_LEVEL)
            << "Token: " << MYCC_PRETTY_PRINT_TOKEN(list.front())
            << " is consumed by [" << caller_ << "] " << _input_file << ":" << line_
            << std::endl;
        return pop_list(list);
    }

  private:
    std::string caller_;
    std::string _input_file;
    int line_;
};

// remove the symbol for the function, then define a new version that instead
// creates a stack temporary instance of Reporter initialized with the caller
#undef pop_list
#define pop_list PopFuncReporter(__FUNCTION__, __FILE__, __LINE__)
#endif

}  // namespace TokenListUtils
}  // namespace Hzcc::Syntax
#endif  // MYCC_SOURCE_SYNTAX_UTILS_TOKEN_LIST_UTILS_H_
