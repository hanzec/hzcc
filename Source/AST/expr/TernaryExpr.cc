//
// Created by chen_ on 2022/3/29.
//
#include "AST/expr/TernaryExpr.h"

#include "AST/type/Type.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
TernaryExpr::TernaryExpr(std::unique_ptr<ASTNode> cond,        // NOLINT
                         std::unique_ptr<ASTNode> true_expr,   // NOLINT
                         std::unique_ptr<ASTNode> false_expr,  // NOLINT
                         const std::pair<int, int>& location)  // NOLINT
    : ASTNode(location),
      _cond(std::move(cond)),
      _true_expr(std::move(true_expr)),
      _false_expr(std::move(false_expr)) {
    /** #####################################################################
     * ### Runtime Assertion                                             ###
     * ##################################################################### */
    HZCC_RUNTIME_CHECK(_cond != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("cond is nullptr", this);
    HZCC_RUNTIME_CHECK(_true_expr != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("true_expr is nullptr", this);
    HZCC_RUNTIME_CHECK(_false_expr != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("false_expr is nullptr", this);
}

const char* AST::TernaryExpr::NodeName() const { return "TernaryExpr"; }
std::shared_ptr<Type> TernaryExpr::RetType() const {
    return _true_expr->RetType();
}
Status TernaryExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::string TernaryExpr::PrintDetail(const std::string& ident) const {
    std::stringstream ss;

    // type
    ss << RetType()->GetName() << std::endl;

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
