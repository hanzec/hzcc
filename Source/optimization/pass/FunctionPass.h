//
// Created by Hanze Chen on 4/11/22.
//
#include "AST/statement/FuncDeclStmt.h"
#include "PassBase.h"
#ifndef HZCC_OPTIMIZATION_PASS_FUNCTION_PASS_H
#define HZCC_OPTIMIZATION_PASS_FUNCTION_PASS_H
namespace Hzcc {
namespace AST {
class FuncDeclStmt;
}
namespace Pass {
class FunctionPass : public PassBase {
  public:
    bool IsFunctionPass() override { return true; }
    virtual std::pair<bool, bool> RunOnFunction(
        std::unique_ptr<AST::FuncDeclStmt>& F) = 0;
};
}  // namespace Pass
}  // namespace Hzcc
#endif  // HZCC_OPTIMIZATION_PASS_FUNCTION_PASS_H
