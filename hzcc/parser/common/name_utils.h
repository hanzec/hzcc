//
// Created by chen_ on 2022/12/17.
//
#ifndef HZCC_NAME_UTILS_H
#define HZCC_NAME_UTILS_H
#include "enums.h"
#include "keywords.h"
#include "utils/constexpr_utils.h"
namespace hzcc ::parser_common {

ALWAYS_INLINE bool IsPrimitiveType(std::string_view str) {
    return ::hzcc::utils::search_table(
               str.data(), hzcc::parser_common::kPrimitiveTypeTable) != -1;
}

/**
 * @Brief: Check if the giving char is primitive type.
 * @param str the char to be checked.
 * @return true if the char is the special symbol, false otherwise.
 */
template <std::size_t N>
constexpr ALWAYS_INLINE bool IsPrimitiveType(const char (&str)[N]) {
    return ::hzcc::utils::search_table(
               str, hzcc::parser_common::kPrimitiveTypeTable) != -1;
}

ALWAYS_INLINE PrimitiveType GetPrimitiveType(std::string_view str) {
    auto ret = magic_enum::enum_cast<PrimitiveType>(::hzcc::utils::search_table(
        str.data(), hzcc::parser_common::kPrimitiveTypeTable));

    INTERNAL_LOG_IF(FATAL, !ret.has_value()) << "Invalid PrimitiveType";

    return ret.value();
}

template <std::size_t N>
constexpr ALWAYS_INLINE PrimitiveType GetPrimitiveType(const char (&str)[N]) {
    constexpr auto ret =
        magic_enum::enum_cast<PrimitiveType>(::hzcc::utils::search_table(
            str, hzcc::parser_common::kPrimitiveTypeTable));

    static_assert(ret.has_value(), "Invalid PrimitiveType");
    return ret.value();
}
}  // namespace hzcc::parser_common
#endif  // HZCC_NAME_UTILS_H
