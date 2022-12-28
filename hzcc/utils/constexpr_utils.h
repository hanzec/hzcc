//
// Created by hanzech on 12/19/22.
//
#include <array>
#include <cstddef>

#include "macro.h"
#ifndef HZCC_CONSTEXPR_UTILS_H
#define HZCC_CONSTEXPR_UTILS_H
namespace hzcc::utils {
/**
 * @Brief: internal_status constexpr wrap for searching an std::array with
 * constexpr, which is a constexpr version of std::find_if
 * @note: Since all table's size is not so large, will trying to unroll the loop
 * if possible.
 * @tparam Type the type of the array element.
 * @tparam N the size of the array.
 * @param table the array.
 * @param str the string to be searched.
 * @return the index of the string in the array.
 */

#if !defined(__clang__) && defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("unroll-loops")
#endif
template <class Type, unsigned long Size>
constexpr ALWAYS_INLINE int search_table(Type str,
                                         const std::array<Type, Size> &table) {
#ifdef __clang__
#pragma clang loop unroll(full)
#endif
    for (int i = 0; i < Size; ++i) {
        if constexpr (std::is_same_v<Type, const char *>) {
            if (std::strcmp(table[i], str) == 0) {
                return i;
            }
        } else {
            if (table[i] == str) {
                return i;
            }
        }
    }
    return -1;
}
#if !defined(__clang__) && defined(__GNUC__)
#pragma GCC pop_options
#endif

template <typename Enumeration>
constexpr auto as_integer(Enumeration const value) ->
    typename std::underlying_type<Enumeration>::type {
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

}  // namespace hzcc::utils
#endif  // HZCC_CONSTEXPR_UTILS_H
