//
// Created by Hanze Chen on 2022/3/29.
//
#include "OperatorBase.h"
namespace hzcc::ast {
CommaExpr::CommaExpr(const Position& loc,        // NOLINT
                     std::unique_ptr<Expr> lhs,  // NOLINT
                     std::unique_ptr<Expr> rhs)  // NOLINT
    : OperatorBase(loc, "CommaExpr", std::move(rhs), std::move(lhs)) {}

Status CommaExpr::visit(Visitor& visitor) { return visitor.visit(this); }
void CommaExpr::PrintDetail(std::ostream& out, const std::string& ident) const {

}
}  // namespace hzcc::ast
