//
// Created by Hanze Chen on 2022/3/29.
//
#include <algorithm>

#include "OperatorBase.h"
#include "ast/DeduceValue.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
constexpr static std::array<const char*,
                            ArithmeticType::kArithmeticType_ENUM_SIZE>
    kArithmeticStr = {"add", "sub", "mul", "div", "mod"};

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
    if (!(GetLHS()->GetDeducedValue().has_value() &&
          GetRHS()->GetDeducedValue().has_value())) {
        return std::nullopt;
    }

    switch (this->_type) {
        case ArithmeticType::kArithmeticType_Add:
            return GetLHS()->GetDeducedValue().value() +
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Sub:
            return GetLHS()->GetDeducedValue().value() -
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Mul:
            return GetLHS()->GetDeducedValue().value() *
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Div:
            return GetLHS()->GetDeducedValue().value() /
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Mod:
            return GetLHS()->GetDeducedValue().value() %
                   GetRHS()->GetDeducedValue().value();
        default:
            return std::nullopt;
    }
}

Status ArithmeticExpr::visit(Visitor& visitor) { return visitor.visit(this); }

ArithmeticType ArithmeticExpr::GetOpType() const { return _type; }

void ArithmeticExpr::PrintDetail(std::ostream& out,
                                 const std::string& ident) const {
    // print node info
    out << kArithmeticStr[_type] << ' ' << GetLHS()->retType()->Name();

    // print LHS and RHS info
    std::string new_ident(ident);
    std::replace(new_ident.begin(), new_ident.end(), '`', ' ');
    GetLHS()->Dump(out, new_ident + " |");
    GetRHS()->Dump(out, new_ident + " `");
};

}  // namespace hzcc::ast
