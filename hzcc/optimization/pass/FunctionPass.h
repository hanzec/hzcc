//
// Created by Hanze Chen on 4/11/22.
//
#include "AST/statement/FuncDeclStmt.h"
#include "PassBase.h"
#ifndef HZCC_OPTIMIZATION_PASS_FUNCTION_PASS_H
#define HZCC_OPTIMIZATION_PASS_FUNCTION_PASS_H
namespace hzcc {
namespace AST {
/**
 * @brief Forward declaration of a function declaration in the legacy AST.
 */
class FuncDeclStmt;
}
namespace Pass {
/**
 * @brief Optimization pass operating on one function and reporting success and modification.
 */
class FunctionPass : public PassBase {
  public:
    /**
     * @brief Report whether this object is a function-level optimization pass.
     * @return True when the stated condition holds; otherwise false.
     */
    bool IsFunctionPass() override { return true; }
    /**
     * @brief Transform a function body and report whether the pass succeeded and changed it.
     * @param F Owned function to inspect and modify in place.
     * @return Pair of pass success and whether the function was modified.
     */
    virtual std::pair<bool, bool> RunOnFunction(
        std::unique_ptr<AST::FuncDeclStmt>& F) = 0;
};
}  // namespace Pass
}  // namespace Hzcc
#endif  // HZCC_OPTIMIZATION_PASS_FUNCTION_PASS_H
