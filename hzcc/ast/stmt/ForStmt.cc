//
// Created by chen_ on 2022/3/29.
//
#include "ast/Stmt.h"
namespace hzcc::ast {
ForStmt::ForStmt(const Position& location)  // NOLINT
    : Stmt(location, "ForStmt"){
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
}

Status ForStmt::visit(Visitor& visitor) { return visitor.visit(this); }

const std::unique_ptr<Stmt>& ForStmt::init_stmt() { return _init; }
const std::unique_ptr<Stmt>& ForStmt::cond_stmt() { return _cond; }
const std::unique_ptr<Stmt>& ForStmt::step_stmt() { return _step; }
const std::unique_ptr<Stmt>& ForStmt::body_stmt() { return _body; }
}  // namespace hzcc::ast
