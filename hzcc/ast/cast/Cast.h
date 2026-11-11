//
// Created by chen_ on 2022/6/13.
//
#ifndef MYCC_SOURCE_AST_CAST_CASTAPPLIER_H_
#define MYCC_SOURCE_AST_CAST_CASTAPPLIER_H_
#include <memory>         // for unique_ptr, shared_ptr
#include <string>         // for string
#include <unordered_map>  // for unordered_map

#include "ICastRule.h"
#include "utils/factory.h"          // for AutoRegFactory, HZCC_REGI_PRODUCT...
#include "utils/status/statusor.h"  // for StatusOr

namespace hzcc::ast {
class Expr;
class Type;
class ICastRule;

/**
 * @brief Select registered conversion rules and own their lazily created instances.
 */
class Cast : public utils::AutoRegFactory<ICastRule, std::string> {
  public:
    /**
     * @brief Construct a facade for the shared cast-rule registry.
     */
    Cast() = default;
    /**
     * @brief Destroy this facade without clearing the shared cast-rule cache.
     */
    ~Cast() = default;
    /**
     * @brief Disable copying or moving the cast-rule registry facade.
     */
    Cast(Cast&) = delete;
    /**
     * @brief Disable copying or moving the cast-rule registry facade.
     */
    Cast(Cast&&) = delete;
    /**
     * @brief Disable copying or moving the cast-rule registry facade.
     */
    Cast& operator=(Cast&) = delete;
    /**
     * @brief Disable copying or moving the cast-rule registry facade.
     */
    Cast& operator=(Cast&&) = delete;

    /**
     * @brief Consume an expression and apply registered conversions to its target type.
     * @param require_rvalue Insert an lvalue-to-rvalue node when the source is an lvalue.
     * @param rhs Non-null source expression whose ownership is consumed.
     * @param lhs_type Shared destination type.
     * @return The owned converted expression, or a cast-stage error if no rule succeeds.
     */
    static StatusOr<std::unique_ptr<Expr>> apply(
        bool require_rvalue,                     // NOLINT
        std::unique_ptr<Expr> rhs,               // NOLINT
        QualTypePtr lhs_type);  // NOLINT

  private:
    /**
     * The set of cast rules.
     */
    inline static std::unordered_map<std::string, std::unique_ptr<ICastRule>>
        _rules;
};

#define HZCC_REG_CAST_RULE(Product, name, desc) \
    HZCC_REGI_PRODUCT_INTERNAL(Product, ::hzcc::ast::Cast, name, desc)

}  // namespace hzcc::ast
#endif  // MYCC_SOURCE_AST_CAST_CASTAPPLIER_H_
