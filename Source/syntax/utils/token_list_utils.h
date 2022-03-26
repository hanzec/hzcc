//
// Created by chen_ on 2022/3/24.
//
#include "macro.h"
#include <optional>
#include "lexical/Token.h"
#ifndef MYCC_SOURCE_SYNTAX_UTILS_TOKEN_LIST_UTILS_H_
#define MYCC_SOURCE_SYNTAX_UTILS_TOKEN_LIST_UTILS_H_
namespace Mycc::Syntax::TokenListUtils {
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

ALWAYS_INLINE Lexical::Token pop(std::list<Lexical::Token> &list) {
    auto token = list.front();
    list.pop_front();
    return token;
}

ALWAYS_INLINE Lexical::Token peek(std::list<Lexical::Token> &list) {
    return list.front();
}

ALWAYS_INLINE Lexical::Token peek2(std::list<Lexical::Token> &list) {
    return *(std::next(list.begin()));
}
}  // namespace Mycc
#endif  // MYCC_SOURCE_SYNTAX_UTILS_TOKEN_LIST_UTILS_H_
