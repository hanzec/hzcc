//
// Created by chen_ on 2022/3/29.
//
#include "arithmetic.h"

#include <glog/logging.h>

#include <algorithm>

#include "AST/DeduceValue.h"
#include "AST/type/Type.h"
#include "lexical/Token.h"
namespace Hzcc::AST {
constexpr static std::array<const char*,
                            ArithmeticType::kArithmeticType_ENUM_SIZE>
    kArithmeticStr = {"add", "sub", "mul", "div", "mod"};

const char* ArithmeticExpr::NodeName() const { return "ArithmeticExpr"; }

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
ArithmeticExpr::ArithmeticExpr(const Lexical::Token& type,
                               std::unique_ptr<ASTNode> lhs,
                               std::unique_ptr<ASTNode> rhs)
    : OperatorBase(type.Location(), std::move(lhs), std::move(rhs)) {
    switch (type.Value()[0]) {
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
            DLOG(FATAL) << "Unknown arithmetic operator: " << type.Value();
    }
}
Status ArithmeticExpr::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
ArithmeticType ArithmeticExpr::GetOpType() const { return _type; }

std::string ArithmeticExpr::PrintAdditionalInfo(
    const std::string& ident) const {
    std::stringstream ss;

    // print node info
    ss << kArithmeticStr[_type] << ' ' << GetLHS()->GetType()->GetName();

    // print LHS and RHS info
    std::string new_ident(ident);
    std::replace(new_ident.begin(), new_ident.end(), '`', ' ');
    ss << std::endl << GetLHS()->Dump(new_ident + " |");
    ss << std::endl << GetRHS()->Dump(new_ident + " `");

    return ss.str();
};

}  // namespace Hzcc::AST
