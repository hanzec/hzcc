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
    kLonglong,
    kLong_double,
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
    ASSIGN = 0,
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    MOD = 5,
    LSHIFT = 6,
    RSHIFT = 7,
    AND = 8,
    OR = 9,
    XOR = 10,
};

enum class TypeCategory {
    kNumerical,
    kPointer,
    kArray,
    kStruct,
    kFuncPtr,
    kEnum,
    kUnion,
    kBuiltin,
    kUnknown,
};

enum class Attribute {
    kCONST,
    kEXTERN,
    kSTATIC,
    kAUTO,
    kVOLATILE,
    kINLINE,
    kRESTRICT,
    kSIGNED,
    kUNSIGNED,
    kREGISTER,
};

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

constexpr const std::array<const char[10], magic_enum::enum_count<Attribute>()>
    kAttributeTable{"const",  "extern",   "static", "auto",     "volatile",
                    "inline", "restrict", "signed", "unsigned", "register"};

constexpr const std::array<const char[15],
                           magic_enum::enum_count<PrimitiveType>()>
    kPrimitiveTypeTable{"int",     "char",      "float",    "double",
                        "void",    "long",      "short",    "bool",
                        "complex", "imaginary", "longlong", "long double"};

constexpr const int kDoubleCharSymbolTableSize = 19;
constexpr std::array<const char *, kDoubleCharSymbolTableSize>
    kDoubleCharSymbol{
        "==", "!=", ">=", "<=", "++", "--", "||", "&&", "<<", ">>",
        "+=", "-=", "*=", "/=", "->", "%=", "&=", "|=", "^="};
}  // namespace

ALWAYS_INLINE constexpr PrimitiveType to_primitive_type(
    const std::string &type) {
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
        return PrimitiveType::kLonglong;
    } else if (type == "long_double") {
        return PrimitiveType::kLong_double;
    } else {
        INTERNAL_LOG(FATAL) << "Unknown primitive type: " << type;
    }
}

ALWAYS_INLINE constexpr Attribute to_attr(TokenType attr) {
    switch (attr) {
        case TokenType::kConst:
            return Attribute::kCONST;
        case TokenType::kExtern:
            return Attribute::kEXTERN;
        case TokenType::kStatic:
            return Attribute::kSTATIC;
        case TokenType::kAuto:
            return Attribute::kAUTO;
        case TokenType::kVolatile:
            return Attribute::kVOLATILE;
        case TokenType::kInline:
            return Attribute::kINLINE;
        case TokenType::kRestrict:
            return Attribute::kRESTRICT;
        case TokenType::kSigned:
            return Attribute::kSIGNED;
        case TokenType::kUnsigned:
            return Attribute::kUNSIGNED;
        case TokenType::kRegister:
            return Attribute::kREGISTER;
        default:
            INTERNAL_LOG(FATAL)
                << "Unknown attribute: " << magic_enum::enum_name(attr);
    }
}

ALWAYS_INLINE constexpr auto to_string(Attribute attr) -> const char (&)[10] {
    return kAttributeTable[magic_enum::enum_integer(attr)];
}

ALWAYS_INLINE constexpr auto to_string(PrimitiveType attr) -> const
    char (&)[15] {
    return kPrimitiveTypeTable[magic_enum::enum_integer(attr)];
}

ALWAYS_INLINE constexpr auto to_string(ArithmeticType attr) -> const
    char (&)[2] {
    return kArithmeticStr[magic_enum::enum_integer(attr)];
}

ALWAYS_INLINE std::string to_string(TokenType tokenType) {
    if ((int)tokenType >= 401 && (int)tokenType <= 500) {
        return parser_common::kKeywordTable[magic_enum::enum_integer(
                                                tokenType) -
                                            401];
    } else if ((int)tokenType >= 351 && (int)tokenType <= 400) {
        return kDoubleCharSymbol[magic_enum::enum_integer(tokenType) - 351];
    } else if ((int)tokenType <= 0xFF) {
        return std::string(1, (char)tokenType);  // NOLINT
    } else if ((int)tokenType >= 301 && (int)tokenType <= 306) {
        return kLiteralsSymbol[magic_enum::enum_integer(tokenType) - 301];
    } else {
        return "Unknown";
    }
}
}  // namespace hzcc
#endif  // HZCC_ENUMS_H
