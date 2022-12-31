//
// Created by chen_ on 2022/12/17.
//
#ifndef HZCC_NAME_UTILS_H
#define HZCC_NAME_UTILS_H
#include <magic_enum.hpp>
#include <string>
#include "Token.h"
#include "enums.h"
#include "keywords.h"
#include "utils/constexpr_utils.h"
namespace hzcc {
namespace parser_common {
//bool IsAttribute(hzcc::Attribute attr);
//
//
//constexpr ALWAYS_INLINE Attribute TokenTypeToAttr(TokenType type) {
//    static_assert(
//        magic_enum::enum_count<Attribute>() ==
//            (magic_enum::enum_integer(TokenType::kATTRIBUTE_END) -
//             magic_enum::enum_integer(TokenType::kATTRIBUTE_START) - 1),
//        "token type and attribute type not match");
//    return static_cast<Attribute>(
//        magic_enum::enum_integer(type) -
//        magic_enum::enum_integer(TokenType::kATTRIBUTE_START));
//}
//
//constexpr ALWAYS_INLINE unsigned int PrimitiveTypeID(const char *str) {
//    return ::hzcc::utils::search_table(
//        str, hzcc::parser_common::kPrimitiveTypeTable);
//}

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

}  // namespace hzcc
#endif  // HZCC_NAME_UTILS_H
