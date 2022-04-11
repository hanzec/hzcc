//
// Created by chen_ on 2022/3/29.
//
#include "AST/expr/conditional.h"

namespace Mycc::AST {

std::string AST::ConditionalExpr::GetNodeName() const {
    return "ConditionalExpr";
}
std::shared_ptr<Type> ConditionalExpr::GetType() const {
    return _true_expr->GetType();
}
void ConditionalExpr::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}
}  // namespace Mycc::AST
