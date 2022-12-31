//
// Created by chen_ on 2022/6/13.
//
#ifndef MYCC_SOURCE_AST_CAST_CASTAPPLIER_H_
#define MYCC_SOURCE_AST_CAST_CASTAPPLIER_H_
#include <unordered_map>

#include "ast/cast/ICastRule.h"
#include "utils/factory.h"
#include "utils/status/statusor.h"
namespace hzcc::ast {
class Cast : public utils::AutoRegFactory<ICastRule, std::string> {
  public:
    /**
     * Delete the default constructor and copy constructor to prevent the class
     * to be copied.
     */
    Cast() = default;
    ~Cast() = default;
    Cast(Cast&) = delete;
    Cast(Cast&&) = delete;
    Cast& operator=(Cast&) = delete;
    Cast& operator=(Cast&&) = delete;

    /**
     * Apply the cast rule rhs the given cast expression.
     * @param rhs The cast expression rhs be applied.
     * @param lhs_type The cast lhs type.
     * @return The cast expression after the cast rule is applied.
     **/
    static StatusOr<std::unique_ptr<Expr>> apply(
        bool require_rvalue,                     // NOLINT
        std::unique_ptr<Expr> rhs,               // NOLINT
        const std::shared_ptr<Type>& lhs_type);  // NOLINT

  private:
    /**
     * The set of cast rules.
     */
    static std::unordered_map<std::string, std::unique_ptr<ICastRule>> _rules;
};

#define HZCC_REG_CAST_RULE(Product, name, desc) \
    HZCC_REGI_PRODUCT_INTERNAL(Product, ::hzcc::ast::Cast, name, desc)

}  // namespace hzcc::ast
#endif  // MYCC_SOURCE_AST_CAST_CASTAPPLIER_H_
