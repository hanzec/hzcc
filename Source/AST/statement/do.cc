//
// Created by chen_ on 2022/3/29.
//
#include "do.h"

namespace Hzcc::AST {

const char* AST::DoStatement::GetNodeName() const { return "DoStatement"; }
Status DoStatement::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
