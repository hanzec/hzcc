//
// Created by chen_ on 2022/3/29.
//
#include "struct.h"

#include "lexical/Token.h"
namespace Mycc::AST {
std::string StructDeclareNode::GetNodeName() const {
    return "StructDeclareNode";
}
StructDeclareNode::StructDeclareNode(const Lexical::Token& token,
                                     std::shared_ptr<StructType> type)
    : DeclNode(token), _type(std::move(type)) {}
}  // namespace Mycc::AST
