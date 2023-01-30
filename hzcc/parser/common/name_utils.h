//
// Created by chen_ on 2022/12/17.
//
#ifndef HZCC_NAME_UTILS_H
#define HZCC_NAME_UTILS_H
#include "keywords.h"
#include "utils/constexpr_utils.h"
namespace hzcc ::parser_common {
/**
 * @Brief: Check if the giving char is primitive type.
 * @param str the char to be checked.
 * @return true if the char is the special symbol, false otherwise.
 */
constexpr ALWAYS_INLINE bool IsPrimitiveType(const char *str) {
    return ::hzcc::utils::search_table(
               str, hzcc::parser_common::kPrimitiveTypeTable) != -1;
}

}  // namespace hzcc::parser_common
#endif  // HZCC_NAME_UTILS_H
