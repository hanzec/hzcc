//
// Created by Hanze Chen on 2022/3/29.
//
#include <algorithm>
#include "ast/DeduceValue.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
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
    INTERNAL_LOG_IF(FATAL, !type.empty())
        << UniqueName() << "type string empty";
    INTERNAL_LOG_IF(FATAL, type.length() == 1)
        << UniqueName() << "type len mismatch";
#pragma clang diagnostic pop
#endif

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '+':
            this->_type = ArithmeticType::kArithmeticType_Add;
            break;
        case '-':
            this->_type = ArithmeticType::kArithmeticType_Sub;
            break;
        case '*':
            this->_type = ArithmeticType::kArithmeticType_Mul;
            break;
        case '/':
            this->_type = ArithmeticType::kArithmeticType_Div;
            break;
        case '%':
            this->_type = ArithmeticType::kArithmeticType_Mod;
            break;
        default:
            INTERNAL_LOG(FATAL)
                << UniqueName()
                << "type: [" + std::string(type) + "] not supported";
    }
}

std::optional<DeduceValue> ArithmeticExpr::GetDeducedValue() const {
    if (!(lhs_c()->GetDeducedValue().has_value() &&
          rhs_c()->GetDeducedValue().has_value())) {
        return std::nullopt;
    }

    switch (this->_type) {
        case ArithmeticType::kArithmeticType_Add:
            return lhs_c()->GetDeducedValue().value() +
                   rhs_c()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Sub:
            return lhs_c()->GetDeducedValue().value() -
                   rhs_c()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Mul:
            return lhs_c()->GetDeducedValue().value() *
                   rhs_c()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Div:
            return lhs_c()->GetDeducedValue().value() /
                   rhs_c()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Mod:
            return lhs_c()->GetDeducedValue().value() %
                   rhs_c()->GetDeducedValue().value();
        default:
            return std::nullopt;
    }
}

Status ArithmeticExpr::visit(Visitor& visitor) { return visitor.visit(this); }

ArithmeticType ArithmeticExpr::op_type() const { return _type; }

}  // namespace hzcc::ast
