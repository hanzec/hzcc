//
// Created by Hanze Chen on 4/11/22.
//

#ifndef MYCC_SOURCE_OPTMIZATION_PASSMANAGER_H_
#define MYCC_SOURCE_OPTMIZATION_PASSMANAGER_H_
#include "pass/FunctionPass.h"
#include "pass/ImmutablePass.h"
#include "pass/ModulePass.h"
#include "pass/PassBase.h"
#include "utils/Status.h"
namespace Hzcc::Pass {
class PassManager {
  public:
    /**
     * @brief Run all function passes in the current pass manager.
     * @param F The function to be run.
     * @return Status the status of the pass.
     */
    virtual Status RunPass(AST::CompilationUnit& F) = 0;
};

}  // namespace Hzcc::Pass

#endif  // MYCC_SOURCE_OPTMIZATION_PASSMANAGER_H_
