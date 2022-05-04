//
// Created by chen_ on 2022/3/24.
//

#ifndef MYCC_SOURCE_SYNTAX_OPERATOR_TYPE_H_
#define MYCC_SOURCE_SYNTAX_OPERATOR_TYPE_H_
#include <unordered_map>
namespace Hzcc::Syntax::Parser {
enum OperatorType {
    kDot,
    kArraySubscripting,
    kPriority,
    kArrow,
    kNeg,
    kTypeCast,
    kSelfPlus,
    kSelfMinus,
    kDeference,
    kReference,
    kLogicNot,
    kBitwiseNot,
    kSizeof,
    kDiv,
    kMult,
    kMod,
    kAdd,
    kSub,
    kBitwiseShiftLeft,
    kBitwiseShiftRight,
    kGreater,
    kGreaterEqual,
    kLess,
    kLessEqual,
    kEqual,
    kNotEqual,
    kBitwiseAnd,
    kBitwiseXor,
    kBitwiseOr,
    kLogicAnd,
    kLogicOr,
    kCondition,
    kAssign,
    kDivAssign,
    kMultAssign,
    kModAssign,
    kAddAssign,
    kSubAssign,
    kBitwiseShiftLeftAssign,
    kBitwiseShiftRightAssign,
    kBitwiseAndAssign,
    kBitwiseXorAssign,
    kBitwiseOrAssign,
    kComma
};
}  // namespace Hzcc::Syntax::Parser
#endif  // MYCC_SOURCE_SYNTAX_OPERATOR_TYPE_H_
