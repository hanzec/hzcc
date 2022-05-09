//
// Created by chen_ on 2022/3/29.
//
#include "AST/expr/conditional.h"

#include "AST/type/Type.h"
namespace Hzcc::AST {

const char* AST::ConditionalExpr::NodeName() const { return "ConditionalExpr"; }
std::shared_ptr<Type> ConditionalExpr::GetType() const {
    return _true_expr->GetType();
}
Status ConditionalExpr::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
std::string ConditionalExpr::PrintAdditionalInfo(
    const std::string& ident) const {
    std::stringstream ss;

    // type
    ss << GetType()->GetName() << std::endl;

    // node
    ss << _cond->Dump(ident + " |") << std::endl;
    ss << _true_expr->Dump(ident + " |") << std::endl;
    ss << _false_expr->Dump(ident + " `") << std::endl;

    return ss.str();
}
std::unique_ptr<ASTNode>& ConditionalExpr::GetTrueExpr() { return _true_expr; }
std::unique_ptr<ASTNode>& ConditionalExpr::GetContStmt() { return _cond; }
std::unique_ptr<ASTNode>& ConditionalExpr::GetFalseExpr() {
    return _false_expr;
}
}  // namespace Hzcc::AST
