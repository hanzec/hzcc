//
// Created by chen_ on 2022/2/3.
//
#include "utils/Status.h"
#ifndef MYCC_SYNTX_H
#define MYCC_SYNTX_H
namespace Hzcc::Syntax {
Status GenerateAST(AST::CompilationUnit& context, std::list<Lexical::Token>& tokens);
}  // namespace Hzcc::Syntax

#endif  // MYCC_SYNTX_H
