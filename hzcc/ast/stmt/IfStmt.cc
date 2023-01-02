
#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

//
// Created by chen_ on 2022/3/29.
//
#include "ast/Stmt.h"
#include "utils/logging.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/status/status.h"

namespace hzcc::ast {

IfStmt::IfStmt(const Position &location, std::unique_ptr<Stmt> cond,
               std::unique_ptr<Stmt> body)  // NOLINT
    : Stmt(location, "IfStmt"),
      _condition(std::move(cond)),
      _if_body_statement(std::move(body)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, body != nullptr)
        << UniqueName() << "body is nullptr";
    INTERNAL_LOG_IF(FATAL, cond != nullptr)
        << UniqueName() << "condition is nullptr";
#endif
};

Status IfStmt::visit(Visitor &visitor) { return visitor.visit(this); }
void IfStmt::set_else(std::unique_ptr<Stmt> else_body) {
    HZCC_RUNTIME_CHECK_BLOCK({
        INTERNAL_LOG_IF(FATAL, else_body == nullptr)
            << UniqueName() << "else is nullptr";
    })
    _else_statement_ = std::move(else_body);
}

void IfStmt::addElseIf(std::unique_ptr<Stmt> Cond, std::unique_ptr<Stmt> Body) {
    HZCC_RUNTIME_CHECK_BLOCK({
        INTERNAL_LOG_IF(FATAL, Cond == nullptr || Body == nullptr)
            << UniqueName() << "Cond or body is nullptr";
    })
    _elseIfs.emplace_back(std::move(Cond), std::move(Body));
}
bool IfStmt::HasElse() const { return _else_statement_ != nullptr; }
bool IfStmt::has_body() const { return true; }
std::unique_ptr<Stmt> &IfStmt::cond_stmt() { return _condition; }
std::unique_ptr<Stmt> &IfStmt::body_stmt() { return _if_body_statement; }
std::vector<std::pair<std::unique_ptr<Stmt>, std::unique_ptr<Stmt>>>
    &IfStmt::else_if_stmts() {
    return _elseIfs;
}
std::unique_ptr<Stmt> &IfStmt::else_stmt() { return _else_statement_; }
}  // namespace hzcc::ast
