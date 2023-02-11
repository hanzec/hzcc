//
// Created by chen_ on 2022/12/17.
//

#ifndef HZCC_LEXICAL_STATIC_TOKEN_TYPE_H
#define HZCC_LEXICAL_STATIC_TOKEN_TYPE_H
namespace hzcc {
enum class PACKED TokenType : uint16_t {
    Add = '+',
    Sub = '-',
    Mul = '*',
    Div = '/',
    Mod = '%',
    Assign = '=',
    LogicalNot = '!',
    LParentheses = '(',
    RParentheses = ')',
    LBracket = '[',
    RBracket = ']',
    Comma = ',',
    SemiColon = ';',
    Colon = ':',
    Dot = '.',
    Greater = '>',
    Less = '<',
    LBrace = '{',
    RBrace = '}',
    QuestionMark = '?',
    BitwiseAnd = '&',
    BitwiseOr = '|',
    BitWiseNot = '~',
    BitwiseXor = '^',
    Reference = '*',
    Dereference = '&',

    /* literal type */
    LitType_START = 300,
    Char_Lit = 302,
    Int_Lit = 303,
    Real_Lit = 304,
    Str_Lit = 305,
    kIdentity = 306,
    LitType_END = 306,

    /* type specifiers */
    TypeSpec_START = 310,
    Int = 310,
    Char = 311,
    Float = 312,
    Double = 313,
    Void = 314,
    Long = 315,
    Short = 316,
    Bool = 317,
    Complex = 318,
    Imaginary = 319,
    Signed = 320,
    Unsigned = 321,
    Auto = 322,
    TypeSpec_END = 322,

    Op_START = 351,
    EQ = 351,
    NE = 352,
    GE = 353,
    LE = 354,
    kSelfIncrement = 355,
    kSelfDecrement = 356,
    kLogicalOr = 357,
    kLogicalAnd = 358,
    kLeftShift = 359,
    kRightShift = 360,
    kAddAssign = 361,
    kSubAssign = 362,
    kMulAssign = 363,
    kDivAssign = 364,
    kArrow = 365,
    kModAssign = 366,
    kLShiftAssign = 367,
    kRShiftAssign = 368,
    kAndAssign = 369,
    kXorAssign = 370,
    kOrAssign = 371,
    Op_END = 371,

    /* Qualifiers */
    Qual_START = 400,
    Const = 400,
    Volatile = 401,
    Restrict = 402,
    Atomic = 403,
    Qual_END = 403,

    /* keywords */
    Keyword_START = 500,
    Break = 500,
    Case = 501,
    Continue = 502,
    Default = 503,
    Do = 504,
    Else = 505,
    Enum = 506,
    Extern = 507,
    For = 508,
    Goto = 509,
    If = 510,
    Return = 511,
    Sizeof = 512,
    Struct = 513,
    Switch = 514,
    Typedef = 515,
    Union = 516,
    While = 517,
    Alignas = 518,
    Alignof = 519,
    Keyword_END = 519,

    kUnknown = 999
};

#define PRIMITIVE_TYPE_CASES   \
    case TokenType::Int:       \
    case TokenType::Char:      \
    case TokenType::Float:     \
    case TokenType::Double:    \
    case TokenType::Void:      \
    case TokenType::Long:      \
    case TokenType::Short:     \
    case TokenType::Bool:      \
    case TokenType::Complex:   \
    case TokenType::Imaginary: \
    case TokenType::Signed:    \
    case TokenType::Unsigned

constexpr ALWAYS_INLINE bool is_keyword(TokenType tokenType) {
    return (int)tokenType >= (int)TokenType::Keyword_START &&
           (int)tokenType <= (int)TokenType::Keyword_END;
}

constexpr ALWAYS_INLINE bool is_qualifier(TokenType tokenType) {
    return (int)tokenType >= (int)TokenType::Qual_START &&
           (int)tokenType <= (int)TokenType::Qual_END;
}

constexpr ALWAYS_INLINE bool is_type_specifier(TokenType tokenType) {
    return (int)tokenType >= (int)TokenType::TypeSpec_START &&
           (int)tokenType <= (int)TokenType::TypeSpec_END;
}

}  // namespace hzcc
#endif
