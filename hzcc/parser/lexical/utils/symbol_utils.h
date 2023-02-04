//
// Created by chen_ on 2022/2/1.
//
#include <array>
#include <cstring>
#include <string>

#include "const_msg.h"
#include "enums.h"
#include "parser/common/keywords.h"

#ifndef HZCC_SYMBOL_UTILS_H
#define HZCC_SYMBOL_UTILS_H
namespace hzcc::lexical::SymbolUtils {

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
template <class CType, class AType, unsigned long ASize,  // NOLINT
          std::enable_if_t<                               // NOLINT
              std::disjunction_v<                         // NOLINT
                  std::is_convertible<AType, CType>,      // NOLINT
                  std::is_convertible<CType, AType>>,     // NOLINT
              int> = 0>                                   // NOLINT
constexpr ALWAYS_INLINE int Iserch_table(const std::array<AType, ASize> &table,
                                         CType str) {
    static_assert(ASize > 0, "Array size must be greater than 0");
    static_assert(ASize <= std::numeric_limits<int>::max(),
                  "Array size must be less than INT_MAX");

#ifdef __clang__
#pragma clang loop unroll(full)
#endif
    for (size_t i = 0; i < ASize; ++i) {
        if constexpr (std::is_same_v<AType, std::string_view>) {
            if (table[i].compare(str) == 0) {
                return i;
            }
        } else if constexpr (std::is_convertible_v<AType, std::string_view>) {
            if (std::string_view(table[i]).compare(str) == 0) {
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
 * @Brief: Get the type of the single char symbol.
 * @param str the char to be searched.
 * @return the index of the symbol in symbol table, and -1 if not found.
 */
ALWAYS_INLINE TokenType GetSymbolType(const char str) {
    auto ret = Iserch_table(parser_common::kSingleCharSymbol, str);

    if (ret == -1) {
        return TokenType::kUnknown;
    } else {
        return static_cast<TokenType>(parser_common::kSingleCharSymbol[ret]);
    }
}

/**
 * @Brief: Check if the char is the special symbol.
 * @param str the char to be checked.
 * @return true if the char is the special symbol, false otherwise.
 */
ALWAYS_INLINE bool IsOperator(const char str) {
    return str == ' ' || TokenType::kUnknown != GetSymbolType(str);
}

/**
 * @Brief: Check if the giving char is primitive type.
 * @param str the char to be checked.
 * @return true if the char is the special symbol, false otherwise.
 */
ALWAYS_INLINE bool IsPrimitiveType(const char *str) {
    return Iserch_table(::hzcc::parser_common::kPrimitiveTypeTable, str) != -1;
}

/**
 * @Brief: Get the type of the symbol.
 * @param str the double char to be searched.
 * @return the index of the symbol in symbol table, and -1 if not found.
 */
ALWAYS_INLINE TokenType GetSymbolType(const char str[2]) {
    // handle if the char is a single char symbol
    if (GetSymbolType(str[1]) == TokenType::kUnknown) {
        return GetSymbolType(str[0]);
    }

    auto ret = Iserch_table(parser_common::kDoubleCharSymbol,
                            std::string_view(str).substr(0, 2));

    if (ret == -1) {
        return GetSymbolType(str[0]);
    } else {
        return static_cast<TokenType>(ret + 351);
    }
}

/**
 * @Brief: Converting char to ASCII Control StatusCode. There are following
 * ASCII Control StatusCode supported:
 *         - \0: NUL
 *         - \a: BEL
 *         - \b: BS
 *         - \f: FF
 *         - \n: LF
 *         - \r: CR
 *         - \t: HT
 *         - \v: VT
 * @param str the char to be converted.
 * @return the ASCII Control StatusCode, will return 0xFF if not found.
 */
ALWAYS_INLINE char ToASCIIControlCode(const char str) {
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
 * @Brief: ASCII Control StatusCode to its string('\0' ->"\\0"). There are
 * following ASCII Control StatusCode supported:
 *         - \0: NUL
 *         - \a: BEL
 *         - \b: BS
 *         - \f: FF
 *         - \n: LF
 *         - \r: CR
 *         - \t: HT
 *         - \v: VT
 * @param str ascii control code to be converted.
 * @return the ASCII Control StatusCode, will return "" if not found.
 */
ALWAYS_INLINE const char *ASCIIControlCodeToString(const char str) {
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

/**
 * @Brief: Check if the string is the special keyword.
 * @param keyword the string to be checked.
 * @return the index of the keyword in keyword table, and -1 if not found.
 */
ALWAYS_INLINE TokenType GetStringKeywordType(const std::string &keyword) {
    auto ret =
        Iserch_table(::hzcc::parser_common::kKeywordTable, keyword.c_str());

    if (ret == -1) {
        return TokenType::kUnknown;
    } else {
        return static_cast<TokenType>(ret + 401);
    }
}
}  // namespace hzcc::lexical::SymbolUtils
#endif  // HZCC_SYMBOL_UTILS_H
