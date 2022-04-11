//
// Created by chen_ on 2022/3/29.
//
#include "continue.h"

namespace Mycc::AST {

std::string AST::ContinueStatement::GetNodeName() const {
    return "ContinueStatement";
}
void ContinueStatement::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}
}  // namespace Mycc::AST
