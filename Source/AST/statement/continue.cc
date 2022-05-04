//
// Created by chen_ on 2022/3/29.
//
#include "continue.h"

namespace Hzcc::AST {

const char* AST::ContinueStatement::GetNodeName() const {
    return "ContinueStatement";
}
Status ContinueStatement::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
ContinueStatement::ContinueStatement(std::pair<int, int> pair_1)
    : ASTNode(pair_1) {}
}  // namespace Hzcc::AST
