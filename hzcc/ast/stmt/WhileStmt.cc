//
// Created by chen_ on 2022/3/29.
//
#include "ast/Stmt.h"

namespace hzcc::ast {
WhileStmt::WhileStmt(const Position& loc, std::unique_ptr<Stmt> cond,
                     std::unique_ptr<Stmt> body)  // NO_LINT
    : Stmt(loc, "WhileStmt"), _cond(std::move(cond)), _body(std::move(body)) {
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

Status WhileStmt::visit(Visitor& visitor) { return visitor.visit(this); }
std::unique_ptr<Stmt>& WhileStmt::CondStmt() { return _cond; }
std::unique_ptr<Stmt>& WhileStmt::BodyStmt() { return _body; }
void WhileStmt::PrintDetail(std::ostream& out, const std::string& ident) const {
    // print condition
    _cond->Dump(out, ident + " |");

    // print body
    _body->Dump(out, ident + " `");
}
}  // namespace hzcc::ast
