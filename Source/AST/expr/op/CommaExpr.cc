//
// Created by Hanze Chen on 2022/3/29.
//
#include "CommaExpr.h"
namespace Hzcc::AST {
CommaExpr::CommaExpr(const std::string_view& type,    // NOLINT
                     std::unique_ptr<ASTNode> lhs,    // NOLINT
                     std::unique_ptr<ASTNode> rhs,    // NOLINT
                     const std::pair<int, int>& loc)  // NOLINT
    : OperatorBase(std::move(rhs), std::move(lhs), loc) {}

const char* AST::CommaExpr::NodeName() const { return "CommaExpr"; }

Status CommaExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
