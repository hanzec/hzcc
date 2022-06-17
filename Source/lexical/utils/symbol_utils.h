//
// Created by chen_ on 2022/2/1.
//
#include <array>
#include <cstring>
#include <string>

#include "keywords.h"
#include "token_type.h"
#include "utils/keywords_utils.h"
#ifndef HZCC_SYMBOL_UTILS_H
#define HZCC_SYMBOL_UTILS_H
namespace Hzcc::Lexical::SymbolUtils {

/**
 * @Brief: Get the type of the single char symbol.
 * @param str the char to be searched.
 * @return the index of the symbol in symbol table, and -1 if not found.
 */
ALWAYS_INLINE TokenType GetSymbolType(const char str) {
    auto ret =
        KeywordsUtils::Iserch_table<const char, kSingleCharSymbolTableSize>(
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

    auto ret =
        KeywordsUtils::Iserch_table<const char *, kDoubleCharSymbolTableSize>(
            Keywords::kDoubleCharSymbol, tmp.c_str());

    if (ret == -1) {
        return GetSymbolType(str[0]);
    } else {
        return static_cast<TokenType>(ret + 351);
    }
}

/**
 * @Brief: Check if the string is the special keyword.
 * @param keyword the string to be checked.
 * @return the index of the keyword in keyword table, and -1 if not found.
 */
ALWAYS_INLINE TokenType GetStringKeywordType(const std::string &keyword) {
    auto ret = KeywordsUtils::Iserch_table<const char *, kKeywordTableSize>(
        Keywords::kKeywordTable, keyword.c_str());

    if (ret == -1) {
        return TokenType::kUnknown;
    } else {
        return static_cast<TokenType>(ret + 401);
    }
}
}  // namespace Hzcc::Lexical::SymbolUtils
#endif  // HZCC_SYMBOL_UTILS_H
