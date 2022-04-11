//
// Created by chen_ on 2022/3/29.
//
#include "arithmetic.h"

#include <glog/logging.h>

#include "AST/DeduceValue.h"
#include "lexical/Token.h"
namespace Mycc::AST {
std::string ArithmeticExpr::GetNodeName() const { return "ArithmeticExpr"; }

bool ArithmeticExpr::IsDeducible() const {
    return GetLHS()->IsDeducible() && GetRHS()->IsDeducible();
}

std::optional<DeduceValue> ArithmeticExpr::GetDeducedValue() const {
    if (!IsDeducible()) {
        return std::nullopt;
    }

    switch (this->_type) {
        case ArithmeticType::kAdd:
            return GetLHS()->GetDeducedValue().value() +
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kSub:
            return GetLHS()->GetDeducedValue().value() -
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kMul:
            return GetLHS()->GetDeducedValue().value() *
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kDiv:
            return GetLHS()->GetDeducedValue().value() /
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kMod:
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
            this->_type = ArithmeticType::kAdd;
            break;
        case '-':
            this->_type = ArithmeticType::kSub;
            break;
        case '*':
            this->_type = ArithmeticType::kMul;
            break;
        case '/':
            this->_type = ArithmeticType::kDiv;
            break;
        case '%':
            this->_type = ArithmeticType::kMod;
            break;
        default:
            DLOG(FATAL) << "Unknown arithmetic operator: " << type.Value();
    }
}
void ArithmeticExpr::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
};

}  // namespace Mycc::AST
