//
// Created by Hanze Chen on 4/11/22.
//
#include "pass/PassBase.h"
#include "utils/status/status.h"
#ifndef HZCC_SOURCE_OPTIMIZATION_PASSMANAGER_H
#define HZCC_SOURCE_OPTIMIZATION_PASSMANAGER_H
namespace hzcc::Pass {
/**
 * @brief Placeholder for a module-level pass; execution is not implemented.
 */
class ModulePass;
/**
 * @brief Optimization pass operating on one function and reporting success and modification.
 */
class FunctionPass;
/**
 * @brief Placeholder for a read-only pass; execution is not implemented.
 */
class ImmutablePass;
/**
 * @brief Interface for running optimization passes over a compilation unit.
 */
class PassManager {
  public:
    /**
     * @brief Run registered function passes over function declarations in the compilation unit.
     * @param F Compilation unit whose functions are processed.
     * @return OK on success, or the first backend/pass error.
     */
    virtual Status RunPass(std::shared_ptr<AST::CompilationUnit> F) = 0;
};

}  // namespace hzcc::Pass

#endif  // HZCC_SOURCE_OPTIMIZATION_PASSMANAGER_H
