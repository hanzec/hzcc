//
// Created by chen_ on 2022/3/28.
//
#include "decl.h"

#include <utility>

#include "lexical/Token.h"
namespace Mycc::AST {
DeclNode::DeclNode(const Lexical::Token& decl_name)
    : ASTNode(decl_name.Location()), _decl_name(decl_name.Value()) {}
}  // namespace Mycc::AST
