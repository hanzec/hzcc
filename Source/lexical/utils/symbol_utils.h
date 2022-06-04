//
// Created by chen_ on 2022/2/1.
//
#include <array>
#include <cstring>
#include <string>

#include "lexical/keywords.h"
#include "lexical/token_type.h"

#ifndef HZCC_SYMBOL_UTILS_H
#define HZCC_SYMBOL_UTILS_H
namespace Hzcc::Lexical::SymbolUtils {
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
 * @Brief: Get the type of the single char symbol.
 * @param str the char to be searched.
 * @return the index of the symbol in symbol table, and -1 if not found.
 */
ALWAYS_INLINE TokenType GetSymbolType(const char str) {
    auto ret = Iserch_table<const char, kSingleCharSymbolTableSize>(
        Keywords::kSingleCharSymbol, str);

    if (ret == -1) {
        return TokenType::kUnknown;
    } else {
        return static_cast<TokenType>(Keywords::kSingleCharSymbol[ret]);
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
    return Iserch_table<const char *, KPermittedTypeTableSize>(
               Keywords::kPrimitiveType, str) != -1;
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

    // create a tmp string
    std::string tmp(str, 2);

    auto ret = Iserch_table<const char *, kDoubleCharSymbolTableSize>(
        Keywords::kDoubleCharSymbol, tmp.c_str());

    if (ret == -1) {
        return GetSymbolType(str[0]);
    } else {
        return static_cast<TokenType>(ret + 351);
    }
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
    auto ret = Iserch_table<const char *, kKeywordTableSize>(Keywords::kKeyword,
                                                             keyword.c_str());

    if (ret == -1) {
        return TokenType::kUnknown;
    } else {
        return static_cast<TokenType>(ret + 401);
    }
}

ALWAYS_INLINE std::string TokenTypeToString(TokenType tokenType) {
    if ((int)tokenType >= 401 && (int)tokenType <= 500) {
        return Keywords::kKeyword[tokenType - 401];
    } else if ((int)tokenType >= 351 && (int)tokenType <= 400) {
        return Keywords::kDoubleCharSymbol[tokenType - 351];
    } else if ((int)tokenType <= 0xFF) {
        return std::string(1, (char)tokenType);  // NOLINT
    } else if ((int)tokenType >= 301 && (int)tokenType <= 306) {
        return Keywords::kLiteralsSymbol[tokenType - 301];
    } else {
        return "Unknown";
    }
}
}  // namespace Hzcc::Lexical::SymbolUtils
#endif  // HZCC_SYMBOL_UTILS_H
