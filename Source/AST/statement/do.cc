//
// Created by chen_ on 2022/3/29.
//
#include "do.h"

namespace Mycc::AST {

std::string AST::DoStatement::GetNodeName() const { return "DoStatement"; }
void DoStatement::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}
}  // namespace Mycc::AST
