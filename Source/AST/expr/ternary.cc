//
// Created by chen_ on 2022/3/29.
//
#include "AST/expr/ternary.h"

#include "AST/type/Type.h"
namespace Hzcc::AST {

const char* AST::TernaryExpr::NodeName() const { return "TernaryExpr"; }
std::shared_ptr<Type> TernaryExpr::GetType() const {
    return _true_expr->GetType();
}
Status TernaryExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::string TernaryExpr::PrintAdditionalInfo(const std::string& ident) const {
    std::stringstream ss;

    // type
    ss << GetType()->GetName() << std::endl;

    // node
    ss << _cond->Dump(ident + " |") << std::endl;
    ss << _true_expr->Dump(ident + " |") << std::endl;
    ss << _false_expr->Dump(ident + " `") << std::endl;

    return ss.str();
}
std::unique_ptr<ASTNode>& TernaryExpr::GetContStmt() { return _cond; }
std::unique_ptr<ASTNode>& TernaryExpr::GetTrueExpr() { return _true_expr; }
std::unique_ptr<ASTNode>& TernaryExpr::GetFalseExpr() { return _false_expr; }
}  // namespace Hzcc::AST
