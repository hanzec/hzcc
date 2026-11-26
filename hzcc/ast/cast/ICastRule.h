//
// Created by chen_ on 2022/6/13.
//
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "utils/status/statusor.h"
#ifndef MYCC_SOURCE_AST_CAST_RULE_ICASTRULE_H_
#define MYCC_SOURCE_AST_CAST_RULE_ICASTRULE_H_
namespace hzcc::ast {
/**
 * @brief Define the applicability test and transformation for one implicit cast rule.
 */
class ICastRule {
  public:
    /**
     * @brief Test whether this rule can convert an expression to a target type.
     * @param rhs Expression to inspect without taking ownership.
     * @param lhs Desired qualified type, shared with the caller.
     * @return true when Apply can construct the conversion.
     */
    virtual bool CouldApplyTo(const ExprPtr& rhs,
                              const QualTypePtr& lhs) = 0;  // NOLINT

    /**
     * @brief Destroy a registered conversion rule through its interface.
     */
    virtual ~ICastRule() = default;

    /**
     * @brief Attempt to construct the cast node for an owned expression.
     * @param node Source owned by the caller until success. Preserve it on failure.
     * @param to Shared destination type.
     * @param require_rvalue Read an lvalue before converting its numerical value.
     * @return An owned cast of the requested type, or the reason the conversion failed.
     */
    virtual StatusOr<std::unique_ptr<CastExpr>> Apply(
        ExprPtr& node, const QualTypePtr& to, bool require_rvalue) = 0;

    /**
     * @brief Construct a cast-rule interface with no retained conversion state.
     */
    ICastRule() = default;
};

}  // namespace hzcc::ast
#endif  // MYCC_SOURCE_AST_CAST_RULE_ICASTRULE_H_
