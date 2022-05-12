//
// Created by chen_ on 2022/2/3.
//
#include "TokenList.h"
#include "utils/Status.h"
#ifndef MYCC_SYNTX_H
#define MYCC_SYNTX_H
namespace Hzcc::Syntax {
Status GenerateAST(TokenList& tokens,
                   std::shared_ptr<AST::CompilationUnit> compile_unit);
}  // namespace Hzcc::Syntax

#endif  // MYCC_SYNTX_H
