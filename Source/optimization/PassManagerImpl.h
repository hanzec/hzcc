//
// Created by chen_ on 2022/4/18.
//

#ifndef MYCC_SOURCE_OPTMIZATION_PASSMANAGERIMPL_H_
#define MYCC_SOURCE_OPTMIZATION_PASSMANAGERIMPL_H_
#include <functional>
#include <list>
#include <memory>
#include <utility>

#include "PassManager.h"
#include "utils/Status.h"
namespace Mycc::Pass {
class PassManagerImpl : public PassManager {
  public:
    /**
     * Delete the default constructor and copy constructor to prevent the class
     * to be copied.
     */
    PassManagerImpl() = delete;
    ~PassManagerImpl() = delete;
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
    Status RunFunctionPass() override;

    /**
     * @brief Helper class for expose the private method outside
     *
     * @param cmd the name of the command
     * @param desc the description of the command
     * @param obj the reference of std::function for creating the command object
     */
    class RegisterHelper {
      protected:
        static bool reg_pass(const std::string_view& cmd,
                             const std::string_view& desc,
                             std::unique_ptr<PassBase> pass) noexcept {
            return reg_pass_internal(std::move(pass), cmd, desc);
        };
    };

  private:
    inline static std::list<
        std::pair<std::string_view, std::shared_ptr<PassBase>>>
        _pass_map{};

    static bool reg_pass_internal(std::unique_ptr<PassBase>,
                                  const std::string_view& command,
                                  const std::string_view& description);
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
        reg_pass(PASS_NAME, PASS_DESC, std::make_unique<T>());
    };

  private:
    const std::string PASS_NAME;
    const std::string PASS_DESC;
};
}  // namespace Mycc::Pass
#endif  // MYCC_SOURCE_OPTMIZATION_PASSMANAGERIMPL_H_
