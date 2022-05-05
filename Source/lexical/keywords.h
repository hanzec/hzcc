#include <array>

#ifndef MYCC_LEXICAL_KEYWORDS_H
#define MYCC_LEXICAL_KEYWORDS_H

namespace Hzcc::Lexical::Keywords {
/**
 * Number of special symbols in C language.
 */
#define kSingleCharSymbolTableSize 23
#define kDoubleCharSymbolTableSize 19
/**
 * The table of string Keywords in C language.
 */
#define kKeywordTableSize 26
#define KAttributeTableSize 10

#define KLiteralsTypeTableSize 6
#define KPermittedTypeTableSize 10

/**
 * Literals Type's table.
 */
constexpr static std::array<const char *, KLiteralsTypeTableSize>
    kLiteralsSymbol{"Type",       "Char",   "Integer",
                    "RealNumber", "String", "Identity"};

/**
 * Special Symbol's table.
 */
constexpr static std::array<const char, kSingleCharSymbolTableSize>
    kSingleCharSymbol{'+', '-', '*', '/', '%', '&', '|', '!',
                      '=', '.', ',', '~', '<', '>', '?', ':',
                      ';', '{', '}', '(', ')', '[', ']'};

constexpr static std::array<const char *, kDoubleCharSymbolTableSize>
    kDoubleCharSymbol{
        "==", "!=", ">=", "<=", "++", "--", "||", "&&", "<<", ">>",
        "+=", "-=", "*=", "/=", "->", "%=", "&=", "|=", "^="};

/**
 * Special Keyword's table.
 */
constexpr static std::array<const char *, kKeywordTableSize> kKeyword{
    "const",
    "struct",
    "for",
    "while",
    "do",
    "if",
    "else",
    "break",
    "continue",
    "return",
    "switch",
    "case",
    "default",
    "typedef"
    "extern",
    "static",
    "goto",
    "sizeof",
    "union",
    "enum",
    "auto",
    "register",
    "volatile",
    "signed",
    "unsigned",
    "inline",
    "restrict"};

constexpr static std::array<const char *, KAttributeTableSize> kAttribute{
    "const",    "extern", "static",   "auto",   "register",
    "volatile", "signed", "unsigned", "inline", "restrict"};

constexpr static std::array<const char *, KPermittedTypeTableSize>
    kPrimitiveType{"int",  "char",  "float", "double",   "void",
                   "long", "short", "_Bool", "_Complex", "_Imaginary"};
}  // namespace Hzcc::Lexical::Keywords
#endif  // MYCC_LEXICAL_KEYWORDS_H
