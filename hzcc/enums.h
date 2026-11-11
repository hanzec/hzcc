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
/**
 * @brief Broad statement categories used by AST nodes.
 */
enum class StmtType { DECL = 0, EMPTY, RETURN, EXPR, CTRL, DEFAULT };

/**
 * @brief Kinds of declarations represented by the AST.
 */
enum class DeclType {
    PARAM = 0,
    FUNC,
    FIELD,
    RECORD,
    VAR,
    COMB_VAR,
};

/**
 * @brief Primitive type identifiers used to index the compiler type tables.
 */
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

/**
 * @brief Kinds of literal values stored in expression nodes.
 */
enum class LiteralType {
    Char = 0,
    Integer = 1,
    Real_number = 2,
    String = 3,
};

/**
 * @brief Comparison operators recognized by relational expressions.
 */
enum class RelationalType {
    LT = 0,
    EQ = 1,
    NE = 2,
    GT = 3,
    LE = 4,
    GE = 5,
};

/**
 * @brief Short-circuit logical operators.
 */
enum class LogicalType {
    AND = 0,
    OR = 1,
};

/**
 * @brief Bitwise and shift operators represented by binary expressions.
 */
enum class BitwiseType {
    AND = 0,
    OR = 1,
    XOR = 2,
    LSHIFT = 3,
    RSHIFT = 4,
};

/**
 * @brief Simple and compound assignment operators.
 */
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

/**
 * @brief Runtime categories used to discriminate type representations.
 */
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

/**
 * @brief C type qualifiers stored on qualified types.
 */
enum class Qualifier { CONST, VOLATILE, RESTRICT, ATOMIC };

/**
 * @brief Arithmetic operators indexed by the operator spelling table.
 */
enum class ArithmeticType {
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    MOD = 4,
};

/**
 * @brief Prefix and postfix operators represented by unary AST nodes.
 */
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

/**
 * @brief Map a supported internal type spelling to its primitive type identifier.
 * @param type Internal spelling, including the legacy longlong and long_double names.
 * @return Matching primitive identifier; an unknown spelling triggers a fatal log.
 * @note The legacy extended names map to Signed and Unsigned in the current enum.
 */
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

/**
 * @brief Convert a qualifier token to its AST qualifier.
 * @param attr One of Const, Volatile, Restrict, or Atomic.
 * @return Corresponding qualifier; a non-qualifier token triggers a fatal log.
 */
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

/**
 * @brief Look up the display spelling for a qualifier.
 * @param attr Valid Qualifier enumerator indexing the associated table.
 * @return Reference to the static, null-terminated spelling.
 */
ALWAYS_INLINE constexpr auto to_string(Qualifier attr) -> const char (&)[10] {
    return kQualifierTable[magic_enum::enum_integer(attr)];
}

/**
 * @brief Look up the display spelling for a primitive type.
 * @param attr Valid PrimitiveType enumerator indexing the associated table.
 * @return Reference to the static, null-terminated spelling.
 */
ALWAYS_INLINE constexpr auto to_string(PrimitiveType attr) -> const char (&)[15] {
    return kPrimitiveTypeTable[magic_enum::enum_integer(attr)];
}

/**
 * @brief Look up the display spelling for a arithmetic operator.
 * @param attr Valid ArithmeticType enumerator indexing the associated table.
 * @return Reference to the static, null-terminated spelling.
 */
ALWAYS_INLINE constexpr auto to_string(ArithmeticType attr) -> const char (&)[2] {
    return kArithmeticStr[magic_enum::enum_integer(attr)];
}

}  // namespace hzcc
#endif  // HZCC_ENUMS_H
