//
// Created by Hanze Chen on 2022/3/29.
//
#include "CommaExpr.h"
namespace Hzcc::AST {
CommaExpr::CommaExpr(const Position& loc, const std::string_view& type,
                     std::unique_ptr<ASTNode> lhs,
                     std::unique_ptr<ASTNode> rhs)  // NOLINT
    : OperatorBase(loc, std::move(rhs), std::move(lhs)) {}

const char* AST::CommaExpr::NodeName() const { return "CommaExpr"; }

Status CommaExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
