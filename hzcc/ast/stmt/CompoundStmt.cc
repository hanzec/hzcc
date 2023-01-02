//
// Created by chen_ on 2022/3/28.
//
#include <glog/logging.h>
#include <list>
#include <memory>
#include <ostream>
#include <string_view>
#include <utility>

#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/status/status.h"

namespace hzcc::ast {

CompoundStmt::CompoundStmt(const Position& loc) : Stmt(loc, "CompoundStmt") {}

void CompoundStmt::AddStatement(std::unique_ptr<Stmt> statement) {
    LOG_IF(FATAL, statement == nullptr) << "statement is nullptr";
    statements_.push_back(std::move(statement));
}

const std::unique_ptr<Stmt>& CompoundStmt::GetLastStatement() const {
    return statements_.back();
}

std::list<std::unique_ptr<Stmt>>& CompoundStmt::get_body_stmts() {
    return statements_;
}

Status CompoundStmt::visit(Visitor& visitor) { return visitor.visit(this); }

}  // namespace hzcc::ast
