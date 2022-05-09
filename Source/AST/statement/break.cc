//
// Created by chen_ on 2022/3/29.
//
#include "break.h"

namespace Hzcc::AST {

const char* AST::BreakStatement::NodeName() const {
    return "BreakStatement";
}
Status BreakStatement::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
BreakStatement::BreakStatement(std::pair<int, int> pair_1) : ASTNode(pair_1) {}
}  // namespace Hzcc::AST
