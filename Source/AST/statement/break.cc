//
// Created by chen_ on 2022/3/29.
//
#include "break.h"

namespace Mycc::AST {

std::string AST::BreakStatement::GetNodeName() const {
    return "BreakStatement";
}
void BreakStatement::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}
BreakStatement::BreakStatement(std::pair<int, int> pair_1) : ASTNode(pair_1) {}
}  // namespace Mycc::AST
