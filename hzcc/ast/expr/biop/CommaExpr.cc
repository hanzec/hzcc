//
// Created by Hanze Chen on 2022/3/29.
//
#include "ast/expr/Expr.h"
namespace hzcc::ast {
CommaExpr::CommaExpr(const Position& loc,        // NOLINT
                     std::unique_ptr<Expr> lhs,  // NOLINT
                     std::unique_ptr<Expr> rhs)  // NOLINT
    : OperatorBase(loc, "CommaExpr", std::move(rhs), std::move(lhs)) {}

Status CommaExpr::visit(Visitor& visitor) { return visitor.visit(this); }
}  // namespace hzcc::ast
