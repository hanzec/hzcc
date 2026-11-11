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


#if !defined(__clang__) && defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("unroll-loops")
#endif
/**
 * @brief Find the first equal entry in a fixed-size lookup table.
 * @tparam CType Type of the searched value.
 * @tparam AType Table element type.
 * @tparam ASize Nonzero table size representable by int.
 * @param table Table to search; string-like entries are compared by content.
 * @param str Value to locate.
 * @return Zero-based index of the first match, or -1 when absent.
 */
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
 * @brief Classify one punctuation character.
 * @param str Character to look up in the single-character symbol table.
 * @return Its ASCII-valued token category, or TokenType::kUnknown.
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
 * @brief Check whether a character delimits an identifier or number.
 * @param str Character to classify.
 * @return True for a space or a recognized single-character symbol.
 */
ALWAYS_INLINE bool is_operator(const char str) {
    return str == ' ' || TokenType::kUnknown != GetSymbolType(str);
}

/**
 * @brief Resolve a keyword spelling to its token category.
 * @param keyword Exact keyword spelling to look up.
 * @return Matching keyword token, or TokenType::kUnknown when absent.
 */
ALWAYS_INLINE TokenType is_keyword(std::string_view keyword) {
    auto ret = Iserch_table(::hzcc::kKeywordTable, keyword);

    if (__builtin_expect(ret == -1, 0)) {
        return TokenType::kUnknown;
    } else {
        return static_cast<TokenType>(ret +
                                      magic_enum::enum_integer(TokenType::Keyword_START));
    }
}

/**
 * @brief Resolve a C qualifier spelling to its token category.
 * @param str Exact qualifier spelling to look up.
 * @return Matching qualifier token, or TokenType::kUnknown when absent.
 */
constexpr ALWAYS_INLINE TokenType is_qualifier(std::string_view str) {
    auto ret = Iserch_table(hzcc::kQualifierTable, str);

    if (__builtin_expect(ret == -1, 0)) {
        return TokenType::kUnknown;
    } else {
        return static_cast<TokenType>(ret +
                                      magic_enum::enum_integer(TokenType::Qual_START));
    }
}

/**
 * @brief Resolve a type-specifier spelling to its token category.
 * @param str Exact type-specifier spelling to look up.
 * @return Matching type-specifier token, or TokenType::kUnknown when absent.
 */
constexpr ALWAYS_INLINE TokenType is_type_specifier(std::string_view str) {
    auto ret = Iserch_table(hzcc::kTypeSpecifier, str);

    if (__builtin_expect(ret == -1, 0)) {
        return TokenType::kUnknown;
    } else {
        return static_cast<TokenType>(
            ret + magic_enum::enum_integer(TokenType::TypeSpec_START));
    }
}

/**
 * @brief Prefer a two-character operator, falling back to the first character.
 * @param str View whose first two characters can be read.
 * @return Matching multi-character category, or the classification of str[0].
 * @pre The view contains at least two characters.
 */
ALWAYS_INLINE TokenType GetSymbolType(std::string_view str) {
    // handle if the char is a single char symbol
    if (GetSymbolType(str[1]) == TokenType::kUnknown) {
        return GetSymbolType(str[0]);
    }

    auto ret = Iserch_table(hzcc::kOperatorTable, str);

    if (ret == -1) {
        return GetSymbolType(str[0]);
    } else {
        return static_cast<TokenType>(ret + 351);
    }
}

/**
 * @brief Decode the supported single-letter C control-character escapes.
 * @param str Escape letter following the backslash.
 * @return Decoded character, or static_cast<char>(0xFF) for unsupported escapes.
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
 * @brief Encode a supported ASCII control character as a C escape spelling.
 * @param str Control character to encode.
 * @return Pointer to static escape text, or an empty string for unsupported characters.
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
}  // namespace hzcc::lexical::SymbolUtils
#endif  // HZCC_SYMBOL_UTILS_H
