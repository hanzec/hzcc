//
// Created by chen_ on 2022/4/18.
//

#ifndef HZCC_OPTIMIZATION_PASS_PASSBASE_H
#define HZCC_OPTIMIZATION_PASS_PASSBASE_H
namespace hzcc::Pass {
/**
 * @brief Polymorphic base identifying the category of an optimization pass.
 */
class PassBase {
  public:
    /**
     * @brief Release PassBase state.
     */
    virtual ~PassBase() = default;
    /**
     * @brief Report whether this object is a module-level optimization pass.
     * @return True when the stated condition holds; otherwise false.
     */
    virtual bool IsModulePass() { return false; }
    /**
     * @brief Report whether this object is a function-level optimization pass.
     * @return True when the stated condition holds; otherwise false.
     */
    virtual bool IsFunctionPass() { return false; }
    /**
     * @brief Report whether this object is a read-only optimization pass.
     * @return True when the stated condition holds; otherwise false.
     */
    virtual bool IsImmutablePass() { return false; }
};
}  // namespace hzcc::Pass
#endif  // HZCC_OPTIMIZATION_PASS_PASSBASE_H
