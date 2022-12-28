//
// Created by chen_ on 2022/3/29.
//
#include "ast/Stmt.h"

#include "ast/type/Type.h"
namespace hzcc::ast {
EmptyStmt::EmptyStmt() : Stmt({0, 0},"EmptyStmt") {}

Status EmptyStmt::visit(Visitor& visitor) { return visitor.visit(this); }

bool EmptyStmt::IsEmptyStmt() const { return true; }
}  // namespace hzcc::ast
