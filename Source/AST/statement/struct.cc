//
// Created by chen_ on 2022/3/29.
//
#include "struct.h"

#include "lexical/Token.h"
namespace Mycc::AST {
std::string StructDeclareNode::GetNodeName() const {
    return "StructDeclareNode";
}
StructDeclareNode::StructDeclareNode(const std::string& name,
                                     const Lexical::Token& token,
                                     std::shared_ptr<StructType> type)
    : DeclNode(token, name), _type(std::move(type)) {}
void StructDeclareNode::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}
}  // namespace Mycc::AST
