//
// Created by chen_ on 2022/4/18.
//

#ifndef MYCC_SOURCE_OPTIMIZATION_PASS_PASSBASE_H_
#define MYCC_SOURCE_OPTIMIZATION_PASS_PASSBASE_H_
namespace Mycc::Pass {
class PassBase {
  public:
    virtual ~PassBase() = default;
    virtual bool IsModulePass() { return false; }
    virtual bool IsFunctionPass() { return false; }
    virtual bool IsImmutablePass() { return false; }
};
}  // namespace Mycc::Pass
#endif  // MYCC_SOURCE_OPTIMIZATION_PASS_PASSBASE_H_
