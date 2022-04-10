//
// Created by chen_ on 2022/3/28.
//
#include "decl.h"

#include <utility>

#include "lexical/Token.h"
namespace Mycc::AST {
DeclNode::DeclNode(const Lexical::Token& decl_name)
    : ASTNode(decl_name.Location()), _decl_name(decl_name.Value()) {}
DeclNode::DeclNode(const Lexical::Token& decl_loc, std::string  decl_name)
    : ASTNode(decl_loc.Location()), _decl_name(std::move(decl_name)) {}
void DeclNode::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}
}  // namespace Mycc::AST
