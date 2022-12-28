//
// Created by chen_ on 2022/4/18.
//
#include <functional>
#include <list>
#include <memory>
#include <unordered_set>
#include <utility>

#include "AST/CompilationUnit.h"
#include "PassManager.h"
#include "utils/factory.h"
#include "utils/logging.h"
#include "utils/status/status.h"

#ifndef HZCC_OPTIMIZATION_PASSMANAGER_IMPL_H
#define HZCC_OPTIMIZATION_PASSMANAGER_IMPL_H
namespace hzcc::Pass {
class PassManagerImpl : public PassManager,
                        public utils::AutoRegFactory<PassBase, std::string> {
  public:
    /**
     * @brief auto_factory a command
     *
     * @param command_name the name of the command
     * @param command_ptr the pointer to the command
     */
    Status RunPass(std::shared_ptr<AST::CompilationUnit> F) override;
};

/**
 * @brief Pass register
 * @tparam T the type of the pass need to be registered
 */
template <typename T>
class PassRegister : public utils::internal::Register<T, PassManagerImpl> {
  public:
    PassRegister(const std::string& name, const std::string& description)
        : utils::internal::Register<T, PassManagerImpl>(name, description) {}
};

}  // namespace hzcc::Pass
#endif  // HZCC_OPTIMIZATION_PASSMANAGER_IMPL_H
