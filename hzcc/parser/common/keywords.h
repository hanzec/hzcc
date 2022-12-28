//
// Created by chen_ on 2022/12/17.
//
#ifndef HZCC_COMMON_KEYWORDS_H
#define HZCC_COMMON_KEYWORDS_H

#include <array>

#include "macro.h"
#include "token_type.h"

namespace hzcc::parser_common {
constexpr const int kKeywordTableSize = 17;
constexpr const int kAttributeTableSize = 10;
constexpr const int KPermittedTypeTableSize = 5;
/**
 * Special Keyword's table.
 */
static constexpr std::array<const char *, kKeywordTableSize> kKeywordTable{
    "struct", "for",      "while",  "do",   "if",      "else",
    "break",  "continue", "switch", "case", "default", "typedef",
    "goto",   "sizeof",   "union",  "enum", "return"};

constexpr const std::array<const char *, kAttributeTableSize> kAttributeTable{
    "const",  "extern",   "static", "auto",     "volatile",
    "inline", "restrict", "signed", "unsigned", "register"};

constexpr const std::array<const char *, KPermittedTypeTableSize>
    kPrimitiveTypeTable{"char", "int", "float", "double", "void"};

/**
 * Number of special symbols in C language.
 */
static constexpr const int kSingleCharSymbolTableSize = 23;
static constexpr const int kDoubleCharSymbolTableSize = 19;
/**
 * The table of string keywords in C language.
 */
static constexpr const int KLiteralsTypeTableSize = 6;

/**
 * Literals retType's table.
 */
constexpr static std::array<const char *, KLiteralsTypeTableSize>
    kLiteralsSymbol{"retType",    "Char",   "Integer",
                    "RealNumber", "String", "Identity"};

/**
 * Special Symbol's table.
 */
constexpr std::array<const char, kSingleCharSymbolTableSize> kSingleCharSymbol{
    '+', '-', '*', '/', '%', '&', '|', '!', '=', '.', ',', '~',
    '<', '>', '?', ':', ';', '{', '}', '(', ')', '[', ']'};

constexpr std::array<const char *, kDoubleCharSymbolTableSize>
    kDoubleCharSymbol{
        "==", "!=", ">=", "<=", "++", "--", "||", "&&", "<<", ">>",
        "+=", "-=", "*=", "/=", "->", "%=", "&=", "|=", "^="};

constexpr std::array<TokenType, kKeywordTableSize> kReversedKeywordTable{
    TokenType::kStruct, TokenType::kFor,      TokenType::kWhile,
    TokenType::kDo,     TokenType::kIf,       TokenType::kElse,
    TokenType::kBreak,  TokenType::kContinue, TokenType::kSwitch,
    TokenType::kCase,   TokenType::kDefault,  TokenType::kTypedef,
    TokenType::kGoto,   TokenType::kSizeOf,   TokenType::kUnion,
    TokenType::kEnum,   TokenType::kReturn};

constexpr std::array<TokenType, kAttributeTableSize> kReversedAttributeTable{
    TokenType::kConst,    TokenType::kExtern,   TokenType::kStatic,
    TokenType::kAuto,     TokenType::kVolatile, TokenType::kInline,
    TokenType::kRestrict, TokenType::kSigned,   TokenType::kUnsigned,
    TokenType::kRegister};
}  // namespace hzcc::parser_common

#endif  // HZCC_COMMON_KEYWORDS_H
