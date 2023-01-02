//
// Created by chen_ on 2022/3/29.
//
#include <string_view>

#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/status/status.h"

namespace hzcc::ast {
BreakStmt::BreakStmt(const Position& loc) : Stmt(loc,"BreakStmt") {}

Status BreakStmt::visit(Visitor& visitor) { return visitor.visit(this); }
}  // namespace hzcc::ast
