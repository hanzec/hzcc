//
// Created by chen_ on 2022/2/3.
//
#include "utils/Status.h"
#ifndef MYCC_SYNTX_H
#define MYCC_SYNTX_H
namespace Mycc::Syntax {
Status GenerateAST(AST::CompilationUnit& context, std::list<Lexical::Token>& tokens);
}  // namespace Mycc::Syntax

#endif  // MYCC_SYNTX_H
