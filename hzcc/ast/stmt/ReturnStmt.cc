//
// Created by chen_ on 2022/3/29.
//
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"

namespace hzcc::ast {
ReturnStmt::ReturnStmt(const Position& loc,         // NOLINT
                       std::unique_ptr<Expr> expr)  // NOLINT
    : Stmt(loc, "ReturnStmt"), _return_val(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _return_val != nullptr)
        << UniqueName() << "return statement is nullptr";
#endif
}

void ReturnStmt::PrintDetail(std::ostream& out,
                             const std::string& ident) const {
    // print node if available
    if (_return_val) {
        _return_val->Dump(out, ident + " `");
    }
}
Status ReturnStmt::visit(Visitor& visitor) { return visitor.visit(this); }
std::unique_ptr<Expr>& ReturnStmt::GetReturnVal() { return _return_val; }

}  // namespace hzcc::ast
