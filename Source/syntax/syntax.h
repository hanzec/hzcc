//
// Created by chen_ on 2022/2/3.
//
#include "AST/ast.h"
#include "lexical/LexicalToken.h"
#include "utils/Status.h"


#ifndef MYCC_SYNTX_H
#define MYCC_SYNTX_H
namespace Mycc::Syntax {

Status GenertateAST(AST::ASTContext& context,
                    std::vector<Lexical::LexicalToken>& tokens);

Status SplitToBlocks(std::vector<std::vector<Lexical::LexicalToken>>& blocks,
                     std::vector<Lexical::LexicalToken>& tokens);
}  // namespace Mycc::Syntax

#endif  // MYCC_SYNTX_H
