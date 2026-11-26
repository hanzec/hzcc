//
// Created by Hanze Chen on 2022/3/29.
//
#include <glog/logging.h>

#include <limits>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "ast/DeduceValue.h"
#include "ast/Stmt.h"
#include "ast/expr/Expr.h"
#include "ast/visitor.h"
#include "enums.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc::ast {
ArithmeticExpr::ArithmeticExpr(const Position& loc,        // NOLINT
                               std::string_view type,      // NOLINT
                               std::unique_ptr<Expr> lhs,  // NOLINT
                               std::unique_ptr<Expr> rhs)  // NOLINT
    : OperatorBase(loc, "ArithmeticExpr", std::move(lhs), std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-oop50-cpp"
    LOG_IF(FATAL, type.empty()) << UniqueName() << "type string empty";
    LOG_IF(FATAL, type.length() != 1) << UniqueName() << "type len mismatch";
#pragma clang diagnostic pop
#endif

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '+':
            this->_type = ArithmeticType::ADD;
            break;
        case '-':
            this->_type = ArithmeticType::SUB;
            break;
        case '*':
            this->_type = ArithmeticType::MUL;
            break;
        case '/':
            this->_type = ArithmeticType::DIV;
            break;
        case '%':
            this->_type = ArithmeticType::MOD;
            break;
        default:
            LOG(FATAL) << UniqueName()
                       << "type: [" + std::string(type) + "] not supported";
    }
}

std::optional<DeduceValue> ArithmeticExpr::GetDeducedValue() const {
    const auto left = lhs_c()->GetDeducedValue();
    const auto right = rhs_c()->GetDeducedValue();
    if (!left || !right) return std::nullopt;

    const bool integral = left->GetType() != kDeduceValueType_Real_Number &&
                          right->GetType() != kDeduceValueType_Real_Number;
    if (integral) {
        const auto a = left->AsInt();
        const auto b = right->AsInt();
        int64_t result;
        switch (_type) {
            case ArithmeticType::ADD:
                if (__builtin_add_overflow(a, b, &result)) return std::nullopt;
                break;
            case ArithmeticType::SUB:
                if (__builtin_sub_overflow(a, b, &result)) return std::nullopt;
                break;
            case ArithmeticType::MUL:
                if (__builtin_mul_overflow(a, b, &result)) return std::nullopt;
                break;
            case ArithmeticType::DIV:
            case ArithmeticType::MOD:
                if (b == 0 || (a == std::numeric_limits<int64_t>::min() && b == -1)) {
                    return std::nullopt;
                }
                result = _type == ArithmeticType::DIV ? a / b : a % b;
                break;
            default:
                return std::nullopt;
        }
        return DeduceValue(result);
    }

    switch (_type) {
        case ArithmeticType::ADD:
            return *left + *right;
        case ArithmeticType::SUB:
            return *left - *right;
        case ArithmeticType::MUL:
            return *left * *right;
        case ArithmeticType::DIV:
            if (right->AsReal() != 0) return *left / *right;
            return std::nullopt;
        default:
            return std::nullopt;
    }
}
}  // namespace hzcc::ast
