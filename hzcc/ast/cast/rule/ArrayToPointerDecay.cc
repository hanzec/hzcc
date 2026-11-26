//
// Created by chen_ on 2022/6/13.
//
#include "ast/cast/Cast.h"

namespace hzcc::ast {
/**
 * @brief Implement the registered array-to-pointer wrapper conversion.
 */
class ArrayToPointerDecay : public ICastRule {
public:
  /**
   * @brief Test the array and pointer-category conditions.
   * @param rhs Borrowed expression whose type is inspected.
   * @param lhs Shared destination type.
   * @return true for an array and a pointer to the same qualified element type.
   */
  bool CouldApplyTo(const ExprPtr &rhs, const QualTypePtr &lhs) override {
    const auto from = rhs->type();
    if (!from->is<TypeCategory::Array>() || !lhs->is<TypeCategory::Ptr>()) {
      return false;
    }
    const auto element = from->as<ArrayType>()->GetBaseType();
    const auto pointee = lhs->as<PointerType>()->base_type();
    return element->is_same_qualified(*pointee);
  }

  /**
   * @brief Wrap the source expression in an array-decay cast node.
   * @param node Expression whose ownership is transferred to the wrapper.
   * @param to Destination type; this rule currently derives its type from the operand.
   * @param require_rvalue Ignored because array decay does not read a numerical lvalue.
   * @return A successful status result containing the newly owned cast node.
   */
  StatusOr<std::unique_ptr<CastExpr>> Apply(ExprPtr &node, const QualTypePtr &to,
                                            bool require_rvalue) override {
    return std::make_unique<ArrayToPointerDecayCast>(node->loc(), std::move(node));
  }
};

/** @brief Register the ArrayToPointerDecay conversion in the cast-rule factory. */
HZCC_REG_CAST_RULE(ArrayToPointerDecay, "ArrayToPointerDecay", "ArrayToPointerDecayCast")

} // namespace hzcc::ast