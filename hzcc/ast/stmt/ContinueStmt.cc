//
// Created by chen_ on 2022/3/29.
//
#include "ast/Stmt.h"

namespace hzcc::ast {
ContinueStmt::ContinueStmt(const Position& loc) : Stmt(loc, "ContinueStmt") {}

Status ContinueStmt::visit(Visitor& visitor) { return visitor.visit(this); }

}  // namespace hzcc::ast
