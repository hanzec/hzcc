//
// Created by Hanze Chen on 4/11/22.
//

#ifndef MYCC_SOURCE_OPTMIZATION_PASS_FUNCTIONPASS_H_
#define MYCC_SOURCE_OPTMIZATION_PASS_FUNCTIONPASS_H_
#include "AST/statement/function_decl.h"
#include "PassBase.h"
namespace Hzcc {
namespace AST {
class FunctionDeclNode;
}
namespace Pass {
class FunctionPass : public PassBase {
  public:
    bool IsFunctionPass() override { return true; }
    virtual std::pair<bool, bool> RunOnFunction(std::unique_ptr<AST::FunctionDeclNode>& F) = 0;
};
}  // namespace Pass
}  // namespace Hzcc
#endif  // MYCC_SOURCE_OPTMIZATION_PASS_FUNCTIONPASS_H_
