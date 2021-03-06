//
// Created by chen_ on 2022/4/18.
//

#ifndef HZCC_OPTIMIZATION_PASS_PASSBASE_H
#define HZCC_OPTIMIZATION_PASS_PASSBASE_H
namespace Hzcc::Pass {
class PassBase {
  public:
    virtual ~PassBase() = default;
    virtual bool IsModulePass() { return false; }
    virtual bool IsFunctionPass() { return false; }
    virtual bool IsImmutablePass() { return false; }
};
}  // namespace Hzcc::Pass
#endif  // HZCC_OPTIMIZATION_PASS_PASSBASE_H
