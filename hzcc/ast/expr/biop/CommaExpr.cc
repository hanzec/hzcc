//
// Created by Hanze Chen on 2022/3/29.
//
#include <memory>
#include <utility>

#include "ast/expr/Expr.h"
#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/status/status.h"

namespace hzcc::ast {
CommaExpr::CommaExpr(const Position& loc,        // NOLINT
                     std::unique_ptr<Expr> lhs,  // NOLINT
                     std::unique_ptr<Expr> rhs)  // NOLINT
    : OperatorBase(loc, "CommaExpr", std::move(rhs), std::move(lhs)) {}

Status CommaExpr::visit(Visitor& visitor) { return visitor.visit(this); }
}  // namespace hzcc::ast
