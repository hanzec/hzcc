#include <array>

#include "lexical/token_type.h"

#ifndef MYCC_LEXICAL_UTILS_TOKEN_UTILS_H
#define MYCC_LEXICAL_UTILS_TOKEN_UTILS_H

namespace Hzcc::Syntax::TokenUtils {
#define kVariableAttrTableSize 23

constexpr std::array<Lexical::TokenType, kVariableAttrTableSize>
    kAtrributeTable = {
        Lexical::TokenType::kConst,    Lexical::TokenType::kStatic,
        Lexical::TokenType::kRegister, Lexical::TokenType::kVolatile,
        Lexical::TokenType::kSigned,   Lexical::TokenType::kUnsigned,
        Lexical::TokenType::kInline,   Lexical::TokenType::kRestrict};

#if !defined(__clang__) && defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("unroll-loops")
#endif
constexpr ALWAYS_INLINE bool IsAttribute(Lexical::TokenType type) {
#ifdef __clang__
#pragma clang loop unroll(full)
#endif
    for (auto i = 0; i < kVariableAttrTableSize; ++i) {
        if (type == kAtrributeTable[i]) {
            return true;
        }
    }
    return false;
}
#if !defined(__clang__) && defined(__GNUC__)
#pragma GCC pop_options
#endif

}  // namespace Hzcc::Syntax::TokenUtils
#endif  // MYCC_LEXICAL_UTILS_TOKEN_UTILS_H
