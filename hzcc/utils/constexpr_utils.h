//
// Created by hanzech on 12/19/22.
//
#ifndef HZCC_CONSTEXPR_UTILS_H
#define HZCC_CONSTEXPR_UTILS_H
#include <array>
#include <cstring>
#include <initializer_list>

#include "macro.h"
namespace hzcc::utils {


#if !defined(__clang__) && defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("unroll-loops")
#endif
/**
 * @brief internal_status constexpr wrap for searching an std::array with
 * constexpr, which is a constexpr version of std::find_if
 * @note: Since all table's size is not so large, will trying to unroll the loop
 * if possible.
 * @tparam Type the type of the array element.
 * @tparam Size the size of the array.
 * @param table the array.
 * @param str the string to be searched.
 * @return The first matching index, or -1 when no element matches.
 */
template <class Type, unsigned long Size>
constexpr ALWAYS_INLINE int search_table(Type str,
                                         const std::array<Type, Size>& table) {
#ifdef __clang__
#pragma clang loop unroll(full)
#endif
    for (int i = 0; i < Size; ++i) {
        if (table[i] == str) {
            return i;
        }
    }
    return -1;
}
#if !defined(__clang__) && defined(__GNUC__)
#pragma GCC pop_options
#endif


/** @brief Aggregate wrapper allowing a character array to be returned by value.
 * @tparam N Array capacity, including any terminating null character.
 */
template <unsigned N>
struct String {
    char c[N];
};

/** @brief Concatenate null-terminated character arrays at compile time.
 * Source arrays are copied without their individual terminators.
 * @return A String containing concatenated text and one trailing null.
 */
template <unsigned... Len>
constexpr auto concat(const char (&... strings)[Len]) {
    constexpr unsigned N = (... + Len) - sizeof...(Len);
    String<N + 1> result = {};
    result.c[N] = '\0';

    char* dst = result.c;
    for (const char* src : {strings...}) {
        for (; *src != '\0'; src++, dst++) {
            *dst = *src;
        }
    }
    return result;
}
}  // namespace hzcc::utils
#endif  // HZCC_CONSTEXPR_UTILS_H
