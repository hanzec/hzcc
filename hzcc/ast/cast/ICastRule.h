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
     * @return false in the base implementation; subclasses provide their matching rule.
     */
    virtual bool CouldApplyTo(const ExprPtr& rhs,
                              const QualTypePtr& lhs);  // NOLINT

    /**
     * @brief Destroy a registered conversion rule through its interface.
     */
    virtual ~ICastRule() = default;

    /**
     * @brief Attempt to construct the cast node for an owned expression.
     * @param node Source expression whose ownership is consumed.
     * @param to Shared destination type.
     * @return An owned cast expression, or kUnimplemented in this base implementation.
     */
    virtual StatusOr<std::unique_ptr<CastExpr>> Apply(
        std::unique_ptr<Expr> node, const QualTypePtr& to);

    /**
     * @brief Construct a cast-rule interface with no retained conversion state.
     */
    ICastRule() = default;
};

inline bool ICastRule::CouldApplyTo(const ExprPtr& rhs,        // NOLINT
                                    const QualTypePtr& lhs) {  // NOLINT
    return false;
}

inline StatusOr<std::unique_ptr<CastExpr>> ICastRule::Apply(
    std::unique_ptr<Expr> node, const QualTypePtr& to) {
    return Status(StatusCode::kUnimplemented, "Not implemented");
}
}  // namespace hzcc::ast
#endif  // MYCC_SOURCE_AST_CAST_RULE_ICASTRULE_H_
