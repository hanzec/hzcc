//
// Created by chen_ on 2022/3/29.
//
#include "Expr.h"

#include "ast/type/Type.h"
namespace hzcc::ast {
TernaryExpr::TernaryExpr(const Position& location, std::unique_ptr<Expr> cond,
                         std::unique_ptr<Expr> true_expr,
                         std::unique_ptr<Expr> false_expr)  // NOLINT
    : Expr(location, "TernaryExpr"),
      _cond(std::move(cond)),
      _true_expr(std::move(true_expr)),
      _false_expr(std::move(false_expr)) {
    /** #####################################################################
     * ### Runtime Assertion                                             ###
     * ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _cond != nullptr)
        << UniqueName() << "cond is nullptr";
    INTERNAL_LOG_IF(FATAL, _true_expr != nullptr)
        << UniqueName() << "true_expr is nullptr";
    INTERNAL_LOG_IF(FATAL, _false_expr != nullptr)
        << UniqueName() << "false_expr is nullptr";
#endif
}

std::shared_ptr<Type> TernaryExpr::retType() const {
    return _true_expr->retType();
}
Status TernaryExpr::visit(Visitor& visitor) { return visitor.visit(this); }
void TernaryExpr::PrintDetail(std::ostream& out,
                              const std::string& ident) const {
    // type
    out << retType()->Name();

    // node
    _cond->Dump(out, ident + " |");
    _true_expr->Dump(out, ident + " |");
    _false_expr->Dump(out, ident + " `");
}
std::unique_ptr<Expr>& TernaryExpr::GetContStmt() { return _cond; }
std::unique_ptr<Expr>& TernaryExpr::GetTrueExpr() { return _true_expr; }
std::unique_ptr<Expr>& TernaryExpr::GetFalseExpr() { return _false_expr; }

}  // namespace hzcc::ast
