//
// Created by chen_ on 2022/12/17.
//

#ifndef HZCC_LEXICAL_STATIC_TOKEN_TYPE_H
#define HZCC_LEXICAL_STATIC_TOKEN_TYPE_H
namespace hzcc {
enum class PACKED TokenType {
    kEND = 0,
    kAdd = '+',
    kSub = '-',
    kMul = '*',
    kDiv = '/',
    kMod = '%',
    kAssign = '=',
    kLogicalNot = '!',
    kLParentheses = '(',
    kRParentheses = ')',
    kLBracket = '[',
    kRBracket = ']',
    kComma = ',',
    kSemiColon = ';',
    kColon = ':',
    kDot = '.',
    kGreater = '>',
    kLess = '<',
    kLBrace = '{',
    kRBrace = '}',
    kQuestionMark = '?',
    kBitwiseAnd = '&',
    kBitwiseOr = '|',
    kBitWiseNot = '~',
    kBitwiseXor = '^',
    kReference = '*',
    kDereference = '&',

    /* Stuff with attributes */

    kType = 301,
    kChar = 302,
    kInteger = 303,
    kReal_number = 304,
    kString = 305,
    kIdentity = 306,

    /* Symbols */
    kEqual = 351,
    kNotEqual = 352,
    kGreaterEqual = 353,
    kLessEqual = 354,
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
    kSelfMinus = 366,
    kModAssign = 367,
    kLShiftAssign = 368,
    kRShiftAssign = 369,
    kAndAssign = 370,
    kXorAssign = 371,
    kOrAssign = 372,

    kATTRIBUTE_START = 400,
    kConst,
    kExtern,
    kStatic,
    kAuto,
    kVolatile,
    kInline,
    kRestrict,
    kSigned,
    kUnsigned,
    kRegister,
    kATTRIBUTE_END,

    /* keywords */
    kTokenType_KEYWORD_START = 500,
    kStruct,
    kFor,
    kWhile,
    kDo,
    kIf,
    kElse,
    kBreak,
    kContinue,
    kReturn,
    kSwitch,
    kCase,
    kDefault,
    kTypedef,
    kGoto,
    kSizeOf,
    kUnion,
    kEnum,
    kAtomic,
    kTokenType_KEYWORD_END,

    kUnknown = 999
};
}  // namespace hzcc::parser_common
#endif
