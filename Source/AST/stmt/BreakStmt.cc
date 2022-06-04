//
// Created by chen_ on 2022/3/29.
//
#include "BreakStmt.h"

namespace Hzcc::AST {
BreakStmt::BreakStmt(const Position& loc) : ASTNode(loc) {}

const char* AST::BreakStmt::NodeName() const { return "BreakStmt"; }
Status BreakStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
