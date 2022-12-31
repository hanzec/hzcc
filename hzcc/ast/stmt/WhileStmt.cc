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
std::unique_ptr<Stmt>& WhileStmt::cond_stmt() { return _cond; }
std::unique_ptr<Stmt>& WhileStmt::body_stmt() { return _body; }
}  // namespace hzcc::ast
