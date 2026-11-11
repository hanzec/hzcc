//
// Created by chen_ on 2022/6/13.
//
#include <cstring>
#include <memory>
#include <utility>

#include "ast/Stmt.h"
#include "ast/cast/Cast.h"
#include "ast/cast/ICastRule.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "utils/factory.h"
#include "utils/status/statusor.h"

namespace hzcc::ast {
/**
 * @brief Implement the registered array-to-pointer wrapper conversion.
 */
class ArrayToPointerDecay : public ICastRule {
public:
  /**
   * @brief Test the implementation's subscript and pointer-category conditions.
   * @param rhs Borrowed expression whose node name and type are inspected.
   * @param lhs Shared destination type.
   * @return true for a pointer-typed ArraySubscriptExpr and a non-pointer destination.
   */
  bool CouldApplyTo(const ExprPtr &rhs, const QualTypePtr &lhs) override {
    /**
     * In order to apply this rule, the rhs has to be an ArraySubscriptExpr
     * ,the rhs has to be a pointer type and lhs has to be not a pointer
     */
    return !lhs->is<TypeCategory::Ptr>() && rhs->type()->is<TypeCategory::Ptr>() &&
           (strcmp(rhs->NodeName().data(), "ArraySubscriptExpr") == 0);
  }

  /**
   * @brief Wrap the source expression in an array-decay cast node.
   * @param node Expression whose ownership is transferred to the wrapper.
   * @param to Destination type; this rule currently derives its type from the operand.
   * @return A successful status result containing the newly owned cast node.
   */
  StatusOr<std::unique_ptr<CastExpr>> Apply(std::unique_ptr<Expr> node, const QualTypePtr &to) override {
    return std::make_unique<ArrayToPointerDecayCast>(node->loc(), std::move(node));
  }
};

/** @brief Register the ArrayToPointerDecay conversion in the cast-rule factory. */
HZCC_REG_CAST_RULE(ArrayToPointerDecay, "ArrayToPointerDecay", "ArrayToPointerDecayCast")

} // namespace hzcc::ast