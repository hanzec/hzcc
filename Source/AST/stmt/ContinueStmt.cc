//
// Created by chen_ on 2022/3/29.
//
#include "ContinueStmt.h"

namespace Hzcc::AST {
ContinueStmt::ContinueStmt(const Position& loc) : ASTNode(loc) {}

const char* AST::ContinueStmt::NodeName() const { return "ContinueStmt"; }
Status ContinueStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }

}  // namespace Hzcc::AST
