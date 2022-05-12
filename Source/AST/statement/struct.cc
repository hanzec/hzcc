//
// Created by chen_ on 2022/3/29.
//
#include "struct.h"

#include "lexical/Token.h"
namespace Hzcc::AST {
const char* StructDeclareNode::NodeName() const { return "StructDeclareNode"; }
StructDeclareNode::StructDeclareNode(const std::string& name,
                                     const Lexical::Token& token,
                                     std::shared_ptr<StructType> type)
    : DeclNode(token, name), _type(std::move(type)) {}
Status StructDeclareNode::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
std::shared_ptr<Type> StructDeclareNode::GetType() const { return _type; }
}  // namespace Hzcc::AST
