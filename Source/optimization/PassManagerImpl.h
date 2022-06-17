//
// Created by chen_ on 2022/4/18.
//
#include <functional>
#include <list>
#include <memory>
#include <unordered_set>
#include <utility>

#include "AST/CompilationUnit.h"
#include "IPassManager.h"
#include "utils/Status.h"
#include "utils/logging.h"

#ifndef HZCC_OPTIMIZATION_PASSMANAGER_IMPL_H
#define HZCC_OPTIMIZATION_PASSMANAGER_IMPL_H
namespace Hzcc::Pass {
class PassManagerImpl : public IPassManager {
  public:
    /**
     * Delete the default constructor and copy constructor to prevent the class
     * to be copied.
     */
    PassManagerImpl() = default;
    ~PassManagerImpl() = default;
    PassManagerImpl(PassManagerImpl&) = delete;
    PassManagerImpl(PassManagerImpl&&) = delete;
    PassManagerImpl& operator=(PassManagerImpl&) = delete;
    PassManagerImpl& operator=(PassManagerImpl&&) = delete;

    /**
     * @brief register a command
     *
     * @param command_name the name of the command
     * @param command_ptr the pointer to the command
     */
    Status RunPass(std::shared_ptr<AST::CompilationUnit> F) override;

    /**
     * @brief Helper class for expose the private method outside
     *
     * @param cmd the name of the command
     * @param desc the description of the command
     * @param obj the reference of std::function for creating the command object
     */
    class RegisterHelper {
      protected:
        static bool reg_pass(const std::string& cmd, const std::string& desc,
                             std::unique_ptr<PassBase> pass) noexcept {
            return reg_pass_internal(std::move(pass), cmd, desc);
        };
    };

  protected:
    static Status RunFunctionPass(std::unique_ptr<AST::FuncDeclStmt>& F);

  private:
    inline static std::unordered_set<std::string> _registered_passes;
    inline static std::list<
        std::pair<std::string_view, std::shared_ptr<PassBase>>>
        _pass_map{};

    static bool reg_pass_internal(std::unique_ptr<PassBase>,
                                  const std::string& command,
                                  const std::string& description);
};

/**
 * @brief Auto register helper template for Command
 *
 * @tparam T the command need to mark as auto regiter
 */
template <typename T, typename std::enable_if<std::is_base_of<PassBase, T>{},
                                              int>::type = 0>
class RegisterPass : private PassManagerImpl::RegisterHelper {
  public:
    RegisterPass(std::string name, std::string desc)
        : PASS_NAME(std::move(name)), PASS_DESC(std::move(desc)) {
        DLOG_ASSERT(reg_pass(PASS_NAME, PASS_DESC, std::make_unique<T>()))
            << "Register pass [" << PASS_NAME << "] failed";
    };

  private:
    const std::string PASS_NAME;
    const std::string PASS_DESC;
};
}  // namespace Hzcc::Pass
#endif  // HZCC_OPTIMIZATION_PASSMANAGER_IMPL_H
