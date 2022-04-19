//
// Created by Hanze Chen on 4/11/22.
//

#ifndef MYCC_SOURCE_OPTMIZATION_PASS_FUNCTIONPASS_H_
#define MYCC_SOURCE_OPTMIZATION_PASS_FUNCTIONPASS_H_
#include "AST/statement/function_decl.h"
#include "PassBase.h"
namespace Mycc {
namespace AST {
class FunctionDeclNode;
}
namespace Pass {
class FunctionPass : public PassBase {
  public:
    virtual bool runOnFunction(std::unique_ptr<AST::FunctionDeclNode>& F) = 0;
};
}  // namespace Pass
}  // namespace Mycc
#endif  // MYCC_SOURCE_OPTMIZATION_PASS_FUNCTIONPASS_H_
