#include <array>

#include "parser/common/token_type.h"

#ifndef MYCC_LEXICAL_UTILS_TOKEN_UTILS_H
#define MYCC_LEXICAL_UTILS_TOKEN_UTILS_H

namespace hzcc::syntax::utils {
#define kVariableAttrTableSize 23

constexpr std::array<lexical::TokenType, kVariableAttrTableSize>
    kAtrributeTable = {parser_common::kConst,    parser_common::kStatic,
                       parser_common::kRegister, parser_common::kVolatile,
                       parser_common::kSigned,   parser_common::kUnsigned,
                       parser_common::kInline,   parser_common::kRestrict};

#if !defined(__clang__) && defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("unroll-loops")
#endif
constexpr ALWAYS_INLINE bool IsAttribute(lexical::TokenType type) {
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

}  // namespace hzcc::syntax::utils
#endif  // MYCC_LEXICAL_UTILS_TOKEN_UTILS_H
