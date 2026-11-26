//
// Created by chen_ on 2022/4/18.
//
#include <map>
#include <memory>
#include <utility>

#include "ast/CompilationUnit.h"
#include "PassManager.h"
#include "utils/factory.h"
#include "utils/logging.h"
#include "utils/status/status.h"

#ifndef HZCC_OPTIMIZATION_PASSMANAGER_IMPL_H
#define HZCC_OPTIMIZATION_PASSMANAGER_IMPL_H
namespace hzcc::Pass {
/**
 * @brief Legacy registry-backed manager dispatching function optimization passes.
 */
class PassManagerImpl : public PassManager,
                        public utils::AutoRegFactory<PassBase, std::string> {
  public:
    /**
     * @brief Run registered function passes over function declarations in the compilation unit.
     * @param compile_unit Compilation unit whose functions are processed.
     * @return OK on success, or the first backend/pass error.
     */
    Status RunPass(std::shared_ptr<ast::CompilationUnit> compile_unit) override;

  private:
    /**
     * @brief Run applicable registered passes on one function, stopping at the first failure.
     * @param F Owned function to inspect and modify in place.
     * @return OK on success, or the first backend/pass error.
     */
    Status RunFunctionPass(std::unique_ptr<ast::FuncDeclStmt>& F);
    std::map<std::string, std::unique_ptr<PassBase>> _pass_map;
};

/**
 * @brief Pass register
 * @tparam T the type of the pass need to be registered
 */
template <typename T>
/**
 * @brief Register an optimization pass implementation in the pass-manager factory.
 */
class PassRegister : public utils::internal::Register<T, PassManagerImpl> {
  public:
    /**
     * @brief Initialize PassRegister with the supplied backend state.
     * @param name Variable or symbol name to register or look up.
     * @param description Human-readable pass description stored by the registry.
     */
    PassRegister(const std::string& name, const std::string& description)
        : utils::internal::Register<T, PassManagerImpl>(name, description) {}
};

}  // namespace hzcc::Pass
#endif  // HZCC_OPTIMIZATION_PASSMANAGER_IMPL_H
