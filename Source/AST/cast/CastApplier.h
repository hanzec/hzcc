//
// Created by chen_ on 2022/6/13.
//
#include <type_traits>
#include <unordered_set>

#include "AST/cast/ICastRule.h"
#ifndef MYCC_SOURCE_AST_CAST_CASTAPPLIER_H_
#define MYCC_SOURCE_AST_CAST_CASTAPPLIER_H_
namespace Hzcc::AST {
class CastApplier {
  public:
    /**
     * Delete the default constructor and copy constructor to prevent the class
     * to be copied.
     */
    CastApplier() = default;
    ~CastApplier() = default;
    CastApplier(CastApplier&) = delete;
    CastApplier(CastApplier&&) = delete;
    CastApplier& operator=(CastApplier&) = delete;
    CastApplier& operator=(CastApplier&&) = delete;

    /**
     * Apply the cast rule rhs the given cast expression.
     * @param rhs The cast expression rhs be applied.
     * @param lhs_type The cast lhs type.
     * @return The cast expression after the cast rule is applied.
     **/
    static std::unique_ptr<ASTNode> ApplyRules(
        bool require_rvalue,                   // NOLINT
        std::unique_ptr<ASTNode> rhs,          // NOLINT
        const std::shared_ptr<Type>& lhs_type);  // NOLINT

    /**
     * @brief Helper class for expose the private method outside
     *
     * @param cmd the name of the command
     * @param desc the description of the command
     * @param obj the reference of std::function for creating the command object
     */
    class RegisterHelper {
      protected:
        static bool reg_pass(
            const std::string& cmd,                           // NOLINT
            const std::string& desc,                          // NOLINT
            std::unique_ptr<ICastRule> cast_rule) noexcept {  // NOLINT
            return reg_pass_internal(cmd, desc, std::move(cast_rule));
        };
    };

  private:
    inline static std::unordered_set<std::string> _registered_rules;
    inline static std::list<
        std::pair<std::string_view, std::shared_ptr<ICastRule>>>
        _rules_base{};

    static bool reg_pass_internal(const std::string& command,
                                  const std::string& description,
                                  std::unique_ptr<ICastRule> rule);
};

/**
 * @brief Auto register helper template for Command
 *
 * @tparam T the command need to mark as auto register
 */
template <typename T, typename std::enable_if<std::is_base_of<ICastRule, T>{},
                                              int>::type = 0>
class CastRuleRegister : private CastApplier::RegisterHelper {
  public:
    CastRuleRegister(std::string name, std::string desc) noexcept
        : RULE_NAME(std::move(name)), RULE_DESC(std::move(desc)) {
        DLOG_ASSERT(reg_pass(RULE_NAME, RULE_DESC, std::make_unique<T>()))
            << "Register pass [" << RULE_NAME << "] failed";
    };

  private:
    const std::string RULE_NAME;
    const std::string RULE_DESC;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_CAST_CASTAPPLIER_H_
