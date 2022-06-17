#include <array>

#include "token_type.h"

#ifndef HZCC_LEXICAL_KEYWORDS_H
#define HZCC_LEXICAL_KEYWORDS_H
namespace Hzcc::Keywords {
/**
 * Number of special symbols in C language.
 */
#define kSingleCharSymbolTableSize 23
#define kDoubleCharSymbolTableSize 19
/**
 * The table of string Keywords in C language.
 */
#define kKeywordTableSize 17
#define kAttributeTableSize 10

#define KLiteralsTypeTableSize 6
#define KPermittedTypeTableSize 12

/**
 * Literals RetType's table.
 */
constexpr static std::array<const char *, KLiteralsTypeTableSize>
    kLiteralsSymbol{"RetType",    "Char",   "Integer",
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

/**
 * Special Keyword's table.
 */
constexpr std::array<const char *, kKeywordTableSize> kKeywordTable{
    "struct", "for",      "while",  "do",   "if",      "else",
    "break",  "continue", "switch", "case", "default", "typedef",
    "goto",   "sizeof",   "union",  "enum", "return"};

constexpr std::array<TokenType, kKeywordTableSize> kReversedKeywordTable{
    TokenType::kStruct, TokenType::kFor,      TokenType::kWhile,
    TokenType::kDo,     TokenType::kIf,       TokenType::kElse,
    TokenType::kBreak,  TokenType::kContinue, TokenType::kSwitch,
    TokenType::kCase,   TokenType::kDefault,  TokenType::kTypedef,
    TokenType::kGoto,   TokenType::kSizeOf,   TokenType::kUnion,
    TokenType::kEnum,   TokenType::kReturn};

constexpr std::array<const char *, kAttributeTableSize> kAttributeTable{
    "const",    "extern", "static",   "auto",   "register",
    "volatile", "signed", "unsigned", "inline", "restrict"};

constexpr std::array<TokenType, kAttributeTableSize> kReversedAttributeTable{
    TokenType::kConst,   TokenType::kExtern,   TokenType::kStatic,
    TokenType::kAuto,    TokenType::kRegister, TokenType::kVolatile,
    TokenType::kSigned,  TokenType::kUnsigned, TokenType::kInline,
    TokenType::kRestrict};

constexpr std::array<const char *, KPermittedTypeTableSize> kPrimitiveTypeTable{
    "int",   "char",  "float",    "double",     "void",      "long",
    "short", "_Bool", "_Complex", "_Imaginary", "long long", "long double"};

}  // namespace Hzcc::Keywords
#endif  // HZCC_LEXICAL_KEYWORDS_H
