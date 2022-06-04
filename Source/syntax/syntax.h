//
// Created by chen_ on 2022/2/3.
//
#include "TokenList.h"
#include "utils/Status.h"
#ifndef HZCC_SYNTAX_SYNTAX_H
#define HZCC_SYNTAX_SYNTAX_H
namespace Hzcc::Syntax {
Status GenerateAST(TokenList& tokens,
                   std::shared_ptr<AST::CompilationUnit> compile_unit);
}  // namespace Hzcc::Syntax

#endif  // HZCC_SYNTAX_SYNTAX_H
