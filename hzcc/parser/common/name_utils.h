//
// Created by chen_ on 2022/12/17.
//
#ifndef HZCC_NAME_UTILS_H
#define HZCC_NAME_UTILS_H
#include <magic_enum.hpp>
#include <string>

#include "Token.h"
#include "ast/type/Type.h"
#include "keywords.h"
#include "utils/constexpr_utils.h"
namespace hzcc {
namespace parser_common {
bool IsAttribute(hzcc::ast::Attribute attr);

std::string GetAttributeName(hzcc::ast::Attribute attr);

constexpr ALWAYS_INLINE hzcc::ast::Attribute TokenTypeToAttr(TokenType type) {
    static_assert(
        magic_enum::enum_integer(hzcc::ast::Attribute::kEND) -
                magic_enum::enum_integer(hzcc::ast::Attribute::kSTART) ==
            magic_enum::enum_integer(TokenType::kATTRIBUTE_END) -
                magic_enum::enum_integer(TokenType::kATTRIBUTE_START),
        "token type and attribute type not match");
    return static_cast<hzcc::ast::Attribute>(
        magic_enum::enum_integer(type) -
        magic_enum::enum_integer(TokenType::kATTRIBUTE_START));
}
constexpr ALWAYS_INLINE unsigned int PrimitiveTypeID(const char *str) {
    return ::hzcc::utils::search_table(
        str, hzcc::parser_common::kPrimitiveTypeTable);
}

/**
 * @Brief: Check if the giving char is primitive type.
 * @param str the char to be checked.
 * @return true if the char is the special symbol, false otherwise.
 */
constexpr ALWAYS_INLINE bool IsPrimitiveType(const char *str) {
    return ::hzcc::utils::search_table(
               str, hzcc::parser_common::kPrimitiveTypeTable) != -1;
}
}  // namespace parser_common

ALWAYS_INLINE std::string to_string(TokenType tokenType) {
    if ((int)tokenType >= 401 && (int)tokenType <= 500) {
        return ::hzcc::parser_common::kKeywordTable[magic_enum::enum_integer(
                                                        tokenType) -
                                                    401];
    } else if ((int)tokenType >= 351 && (int)tokenType <= 400) {
        return parser_common::kDoubleCharSymbol[magic_enum::enum_integer(
                                                    tokenType) -
                                                351];
    } else if ((int)tokenType <= 0xFF) {
        return std::string(1, (char)tokenType);  // NOLINT
    } else if ((int)tokenType >= 301 && (int)tokenType <= 306) {
        return parser_common::kLiteralsSymbol[magic_enum::enum_integer(
                                                  tokenType) -
                                              301];
    } else {
        return "Unknown";
    }
}

}  // namespace hzcc
#endif  // HZCC_NAME_UTILS_H
