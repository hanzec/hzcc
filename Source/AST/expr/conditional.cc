//
// Created by chen_ on 2022/3/29.
//
#include "AST/expr/conditional.h"

namespace Hzcc::AST {

const char* AST::ConditionalExpr::GetNodeName() const {
    return "ConditionalExpr";
}
std::shared_ptr<Type> ConditionalExpr::GetType() const {
    return _true_expr->GetType();
}
Status ConditionalExpr::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
}  // namespace Hzcc::AST
