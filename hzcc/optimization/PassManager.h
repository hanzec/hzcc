//
// Created by Hanze Chen on 4/11/22.
//
#include "pass/PassBase.h"
#include "utils/status/status.h"
#ifndef HZCC_SOURCE_OPTIMIZATION_PASSMANAGER_H
#define HZCC_SOURCE_OPTIMIZATION_PASSMANAGER_H
namespace hzcc::Pass {
class ModulePass;
class FunctionPass;
class ImmutablePass;
class PassManager {
  public:
    /**
     * @brief Run all function passes in the current pass manager.
     * @param F The function to be run.
     * @return Status the Status of the pass.
     */
    virtual Status RunPass(std::shared_ptr<AST::CompilationUnit> F) = 0;
};

}  // namespace hzcc::Pass

#endif  // HZCC_SOURCE_OPTIMIZATION_PASSMANAGER_H
