//
// Created by chen_ on 2022/3/29.
//
#include "ast/Stmt.h"
namespace hzcc::ast {
DoStmt::DoStmt(std::unique_ptr<Stmt> cond,  // NOLINT
               std::unique_ptr<Stmt> body,  // NOLINT
               const Position& pos)         // NOLINT
    : Stmt(pos, "DoStmt"), _cond(std::move(cond)), _body(std::move(body)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _body != nullptr)
        << UniqueName() << "body is nullptr";
    INTERNAL_LOG_IF(FATAL, _cond != nullptr)
        << UniqueName() << "condition is nullptr";
#endif
}

Status DoStmt::visit(Visitor& visitor) { return visitor.visit(this); }
std::unique_ptr<Stmt>& DoStmt::CondStmt() { return _cond; }
std::unique_ptr<Stmt>& DoStmt::BodyStmt() { return _body; }
void DoStmt::PrintDetail(std::ostream& out, const std::string& ident) const {
    _cond->Dump(out, ident + " |");
    _body->Dump(out, ident + " `");
}
}  // namespace hzcc::ast
