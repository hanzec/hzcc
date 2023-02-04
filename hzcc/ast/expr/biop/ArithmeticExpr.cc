//
// Created by Hanze Chen on 2022/3/29.
//
#include <glog/logging.h>

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
    LOG_IF(FATAL, !type.empty()) << UniqueName() << "type string empty";
    LOG_IF(FATAL, type.length() == 1) << UniqueName() << "type len mismatch";
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
    if (!(lhs_c()->GetDeducedValue().has_value() &&
          rhs_c()->GetDeducedValue().has_value())) {
        return std::nullopt;
    }

    switch (this->_type) {
        case ArithmeticType::ADD:
            return lhs_c()->GetDeducedValue().value() +
                   rhs_c()->GetDeducedValue().value();
        case ArithmeticType::SUB:
            return lhs_c()->GetDeducedValue().value() -
                   rhs_c()->GetDeducedValue().value();
        case ArithmeticType::MUL:
            return lhs_c()->GetDeducedValue().value() *
                   rhs_c()->GetDeducedValue().value();
        case ArithmeticType::DIV:
            return lhs_c()->GetDeducedValue().value() /
                   rhs_c()->GetDeducedValue().value();
        case ArithmeticType::MOD:
            return lhs_c()->GetDeducedValue().value() %
                   rhs_c()->GetDeducedValue().value();
        default:
            return std::nullopt;
    }
}
}  // namespace hzcc::ast
