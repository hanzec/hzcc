#include "macro.h"

#ifndef MYCC_LEXICAL_TOEKN_TYPE_H
#define MYCC_LEXICAL_TOEKN_TYPE_H

namespace Hzcc::Lexical {
/**
 * @brief The TokenType enum
 * @details The token type enum of the following tokens:
 *              TokenType     |     Token String     | Token Description
 *         ------------------ | -------------------- | -----------------
 *         COMMA            |     ,                 | The token of comma
 *         DOT              |     .                 | The token of dot
 *         SEMI             |     ;                 | The token of semicolon
 *         LPAR             |     (                 | The token of left
 * parenthesis RPAR             |     )                 | The token of right
 * parenthesis LBRAK            |     [                 | The token of left
 * bracket RBRAK            |     ]                 | The token of right
 * bracket LBRACE           |     {                 | The token of left brace
 *         RBRACE           |     }                 | The token of right brace
 *         kRelationalType_Greater               |     >                 | The token of greater
 * than kRelationalType_Less               |     <                 | The token of less than
 *         ASSIGN           |     =                 | The token of assignment
 *         PLUS             |     +                 | The token of plus
 *         MINUS            |     -                 | The token of minus
 *         STAR             |     *                 | The token of star
 *         SLASH            |     /                 | The token of slash
 *         MOD              |     %                 | The token of mod
 *         COLON            |     :                 | The token of colon
 *         QUEST           |     ?                 | The token of question
 *         TILDE            |     ~                 | The token of tilde
 *         PIPE             |     |                 | The token of pipe
 *         AMP              |     &                 | The token of ampersand
 *
 *
 *
 */

enum PACKED TokenType {
    END = 0,
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

    /* Keywords */

    kConst = 401,
    kStruct = 402,
    kFor = 403,
    kWhile = 404,
    kDo = 405,
    kIf = 406,
    kElse = 407,
    kBreak = 408,
    kContinue = 409,
    kReturn = 410,
    kSwitch = 411,
    kCase = 412,
    kDefault = 413,
    kTypedef = 414,
    kExtern = 415,
    kStatic = 416,
    kGoto = 417,
    kSizeOf = 418,
    kUnion = 419,
    kEnum = 420,
    kAuto = 421,
    kRegister = 422,
    kVolatile = 423,
    kSigned = 424,
    kUnsigned = 425,
    kInline = 426,
    kRestrict = 427,

    kUnknown = 999
};

}  // namespace Hzcc::Lexical
#endif  // MYCC_LEXICAL_TOEKN_TYPE_H
