//
// Created by chen_ on 2022/6/13.
//
#include <glog/logging.h>

#include <array>

#include "keywords.h"
#include "macro.h"
#include "options.h"
#ifndef MYCC_SOURCE_UTILS_KEYWORDS_UTILS_H_
#define MYCC_SOURCE_UTILS_KEYWORDS_UTILS_H_
namespace Hzcc::KeywordsUtils {
/**
 * @Brief: internal constexpr wrap for searching an std::array with constexpr,
 * which is a constexpr version of std::find_if
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
template <class Type, int N>
constexpr ALWAYS_INLINE unsigned int Iserch_table(
    const std::array<Type, N> &table, Type str) {
#ifdef __clang__
#pragma clang loop unroll(full)
#endif
    for (size_t i = 0; i < N; ++i) {
        if constexpr (std::is_same_v<Type, const char *>) {
            if (strcmp(table[i], str) == 0) {
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

/**
 * @Brief: Check if the giving TokenType is an attribute.
 * @param type the TokenType to be checked.
 * @return true if the char is the an attribute., false otherwise.
 */
constexpr ALWAYS_INLINE bool IsAttribute(TokenType type) {
    return Iserch_table<TokenType, kAttributeTableSize>(
               Keywords::kReversedAttributeTable, type) != -1;
}

/**
 * @Brief: Check if the giving char is an attribute.
 * @param str the char to be checked.
 * @return true if the char is the an attribute., false otherwise.
 */
constexpr ALWAYS_INLINE bool IsAttribute(const char *str) {
    return Iserch_table<const char *, kAttributeTableSize>(
               Keywords::kAttributeTable, str) != -1;
}



/**
 * @Brief: Check if the giving char is primitive type.
 * @param str the char to be checked.
 * @return true if the char is the special symbol, false otherwise.
 */
constexpr ALWAYS_INLINE bool IsPrimitiveType(const char *str) {
    return Iserch_table<const char *, KPermittedTypeTableSize>(
               Keywords::kPrimitiveTypeTable, str) != -1;
}

/**
 * @Brief: Converting char to ASCII Control Code. There are following ASCII
 * Control Code supported:
 *         - \0: NUL
 *         - \a: BEL
 *         - \b: BS
 *         - \f: FF
 *         - \n: LF
 *         - \r: CR
 *         - \t: HT
 *         - \v: VT
 * @param str the char to be converted.
 * @return the ASCII Control Code, will return 0xFF if not found.
 */
constexpr ALWAYS_INLINE char ToASCIIControlCode(const char str) {
    switch (str) {
        case '0':
            return '\0';
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 't':
            return '\t';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 'v':
            return '\v';
        default:
            return (char)0xFF;
    }
}

/**
 * @Brief: ASCII Control Code to its string('\0' ->"\\0"). There are following
 * ASCII Control Code supported:
 *         - \0: NUL
 *         - \a: BEL
 *         - \b: BS
 *         - \f: FF
 *         - \n: LF
 *         - \r: CR
 *         - \t: HT
 *         - \v: VT
 * @param str ascii control code to be converted.
 * @return the ASCII Control Code, will return "" if not found.
 */
constexpr ALWAYS_INLINE const char *ASCIIControlCodeToString(const char str) {
    switch (str) {
        case '\0':
            return "\\0";
        case '\a':
            return "\\a";
        case '\b':
            return "\\b";
        case '\f':
            return "\\f";
        case '\n':
            return "\\n";
        case '\r':
            return "\\r";
        case '\t':
            return "\\t";
        case '\v':
            return "\\v";
        default:
            return "";
    }
}

ALWAYS_INLINE std::string TokenTypeToString(uint32_t tokenType) {
    if (tokenType >= 401 && tokenType <= 500) {
        return Keywords::kKeywordTable[tokenType - 401];
    } else if (tokenType >= 351 && tokenType <= 400) {
        return Keywords::kDoubleCharSymbol[tokenType - 351];
    } else if (tokenType <= 0xFF) {
        return std::string(1, (char)tokenType);  // NOLINT
    } else if (tokenType >= 301 && tokenType <= 306) {
        return Keywords::kLiteralsSymbol[tokenType - 301];
    } else {
        LOG(FATAL) << (Options::Global_disable_color ? "" : KEnableRed)
                   << "[Internal]"
                   << (Options::Global_disable_color ? "" : KDisableColor)
                   << ": "
                   << "Unknown token type: " << (int)tokenType;
    }
}
}  // namespace Hzcc::KeywordsUtils
#endif  // MYCC_SOURCE_UTILS_KEYWORDS_UTILS_H_
