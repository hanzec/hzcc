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
namespace Mycc::Pass {
class PassManager {
  public:
    virtual Status RunFunctionPass() = 0;
};

}  // namespace Mycc::Pass

#endif  // MYCC_SOURCE_OPTMIZATION_PASSMANAGER_H_
