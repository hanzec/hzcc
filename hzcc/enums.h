//
// Created by chen_ on 2022/12/29.
//

#ifndef HZCC_ENUMS_H
#define HZCC_ENUMS_H
#include "macro.h"
#include "parser/common/keywords.h"
#include "parser/common/token_type.h"
#include "utils/logging.h"

namespace hzcc {
enum class StmtType { DECL = 0, EMPTY, RETURN, EXPR, CTRL, DEFAULT };

enum class DeclType {
    PARAM = 0,
    FUNC,
    FIELD,
    RECORD,
    VAR,
    COMB_VAR,
};

enum class PrimitiveType {
    kInt = 0,
    kChar,
    kFloat,
    kDouble,
    kVoid,
    kLong,
    kShort,
    kBool,
    kComplex,
    kImaginary,
    Signed,
    Unsigned,
};

enum class LiteralType {
    Char = 0,
    Integer = 1,
    Real_number = 2,
    String = 3,
};

enum class RelationalType {
    LT = 0,
    EQ = 1,
    NE = 2,
    GT = 3,
    LE = 4,
    GE = 5,
};

enum class LogicalType {
    AND = 0,
    OR = 1,
};

enum class BitwiseType {
    AND = 0,
    OR = 1,
    XOR = 2,
    LSHIFT = 3,
    RSHIFT = 4,
};

enum class AssignType {
    ASSIGN,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    LSHIFT,
    RSHIFT,
    AND,
    OR,
    XOR,
};

enum class TypeCategory {
    Numerical,
    Ptr,
    Array,
    Struct,
    kFuncPtr,
    kEnum,
    Union,
    kBuiltin,
    kUnknown,
};

enum class Qualifier { CONST, VOLATILE, RESTRICT, ATOMIC };

enum class ArithmeticType {
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    MOD = 4,
};

enum class UnaryType {
    ADD = 0,
    PRE_INC = 1,
    PRE_DEC = 2,
    POST_INC = 3,
    POST_DEC = 4,
    ADDR = 5,
    DEREF = 6,
    NOT = 7,
    SUB = 8,
    BIT_NOT = 9,
};

namespace {
constexpr std::array<const char[2], magic_enum::enum_count<ArithmeticType>()>
    kArithmeticStr = {"+", "-", "*", "/", "%"};

/**
 * Literals type's table.
 */
constexpr const int KLiteralsTypeTableSize = 6;
constexpr std::array<const char *, KLiteralsTypeTableSize> kLiteralsSymbol{
    "type", "Char", "Integer", "RealNumber", "String", "Identity"};

constexpr const std::array<const char[10], magic_enum::enum_count<Qualifier>()>
    kQualifierTable{"const", "volatile", "restrict", "_Atomic"};

constexpr const std::array<const char[15], magic_enum::enum_count<PrimitiveType>()>
    kPrimitiveTypeTable{"int",     "char",      "float",     "double",
                        "void",    "long",      "short",     "bool",
                        "complex", "imaginary", "long long", "long double"};

constexpr const std::array<const char[4],                                 // NOLINT
                           magic_enum::enum_integer(TokenType::Op_END) -  // NOLINT
                               magic_enum::enum_integer(TokenType::Op_START) +
                               1>  // NOLINT
    kOperatorTable{"==", "!=", ">=", "<=", "++", "--",  "||",  "&&", "<<", ">>", "+=",
                   "-=", "*=", "/=", "->", "%=", ">>=", ">>=", "&=", "^=", "|="};

constexpr const std::array<const char *,
                           magic_enum::enum_integer(TokenType::TypeSpec_END) -  // NOLINT
                               magic_enum::enum_integer(TokenType::TypeSpec_START) + 1>
    kTypeSpecifier{"int",    "char",     "float", "double",   "void",
                   "long",   "short",    "_Bool", "_Complex", "_Imaginary",
                   "signed", "unsigned", "auto"};

constexpr const std::array<const char *, 20> kKeywordTable{
    "break",  "case",    "continue", "default", "do",       "else",    "enum",
    "extern", "for",     "goto",     "if",      "return",   "sizeof",  "struct",
    "switch", "typedef", "union",    "while",   "_Alignas", "_Alignof"};
}  // namespace

ALWAYS_INLINE constexpr PrimitiveType to_primitive_type(const std::string_view type) {
    if (type == "int") {
        return PrimitiveType::kInt;
    } else if (type == "char") {
        return PrimitiveType::kChar;
    } else if (type == "float") {
        return PrimitiveType::kFloat;
    } else if (type == "double") {
        return PrimitiveType::kDouble;
    } else if (type == "void") {
        return PrimitiveType::kVoid;
    } else if (type == "long") {
        return PrimitiveType::kLong;
    } else if (type == "short") {
        return PrimitiveType::kShort;
    } else if (type == "bool") {
        return PrimitiveType::kBool;
    } else if (type == "complex") {
        return PrimitiveType::kComplex;
    } else if (type == "imaginary") {
        return PrimitiveType::kImaginary;
    } else if (type == "longlong") {
        return PrimitiveType::Signed;
    } else if (type == "long_double") {
        return PrimitiveType::Unsigned;
    } else {
        LOG(FATAL) << "Unknown primitive type: " << type;
    }
}

ALWAYS_INLINE constexpr Qualifier to_attr(TokenType attr) {
    switch (attr) {
        case TokenType::Const:
            return Qualifier::CONST;
        case TokenType::Volatile:
            return Qualifier::VOLATILE;
        case TokenType::Restrict:
            return Qualifier::RESTRICT;
        case TokenType::Atomic:
            return Qualifier::ATOMIC;
        default:
            LOG(FATAL) << "Token: " << magic_enum::enum_name(attr)
                       << " is not a qualifier";
    }
}

ALWAYS_INLINE constexpr auto to_string(Qualifier attr) -> const char (&)[10] {
    return kQualifierTable[magic_enum::enum_integer(attr)];
}

ALWAYS_INLINE constexpr auto to_string(PrimitiveType attr) -> const char (&)[15] {
    return kPrimitiveTypeTable[magic_enum::enum_integer(attr)];
}

ALWAYS_INLINE constexpr auto to_string(ArithmeticType attr) -> const char (&)[2] {
    return kArithmeticStr[magic_enum::enum_integer(attr)];
}

}  // namespace hzcc
#endif  // HZCC_ENUMS_H
