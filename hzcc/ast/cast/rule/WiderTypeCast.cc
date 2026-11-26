//
// Created by chen_ on 2022/6/13.
//
#include "ast/cast/Cast.h"

namespace hzcc::ast {
/**
 * @brief WiderTypeCast Cast Rule
 * @details This Rule to cast a type to a wider type (e.g. int to long). The
 * WiderTypeCast only support built-in types. And converting rules are using the
 * following rules for these types (rows converted to columns):
 *      |             | short | int | long | long long | char | void | float | double | long double |
 *      | :---------: | :---: | :-: | :--: | :-------: | :--: | :--: | :---: | :----: | :---------: |
 *      |    short    |  NA   | Yes |  Yes |    Yes    |  No  |  No  |  Yes  |  Yes   |     Yes     |
 *      |     int     |  No   | NA  |  Yes |    Yes    |  No  |  No  |  Yes  |  Yes   |     Yes     |
 *      |    long     |  No   | No  |  NA  |    Yes    |  No  |  No  |  Yes  |  Yes   |     Yes     |
 *      |  long long  |  No   | No  |  No  |    NA     |  No  |  No  |  Yes  |  Yes   |     Yes     |
 *      |    char     |  Yes  | Yes |  Yes |    Yes    |  NA  |  No  |  Yes  |  Yes   |     Yes     |
 *      |    void     |  No   | No  |  No  |    No     |  No  |  No  |  No   |  No    |     No      |
 *      |    float    |  No   | No  |  No  |    No     |  No  |  No  |  NA   |  Yes   |    Yes      |
 *      |   double    |  No   | No  |  No  |    No     |  No  |  No  |  No   |  NA    |    Yes      |
 *      | long double |  No   | No  |  No  |    No     |  No  |  No  |  No   |  No    |    NA       |
 */
class WiderTypeCast : public ICastRule {
public:
  /**
   * @brief Compare widening ranks for two numerical type IDs.
   * @param rhs Borrowed source expression.
   * @param lhs Shared destination type.
   * @return true if the source rank is supported and the destination rank is at least as high.
   */
  bool CouldApplyTo(const ExprPtr &rhs, const QualTypePtr &lhs) override { // NOLINT
    const auto from = rhs->type();

    // builtin type only
    if (!from->is<TypeCategory::Numerical>() || !lhs->is<TypeCategory::Numerical>()) {
      return false;
    }
    const int source_rank = from->as<NumericalType>()->widening_rank();
    return source_rank >= 0 && lhs->as<NumericalType>()->widening_rank() >= source_rank;
  }

  /**
   * @brief Build an integral cast for supported primitive source IDs.
   * @param node Source expression whose ownership is consumed on success.
   * @param to Shared destination type retained by the cast.
   * @param require_rvalue Read a numerical lvalue before constructing the cast.
   * @return An owned IntegralCast for the accepted conversion.
   */
  StatusOr<std::unique_ptr<CastExpr>> Apply(ExprPtr &node, const QualTypePtr &to,
                                            bool require_rvalue) override {
    if (require_rvalue && node->IsReturnLValue()) {
      node = std::make_unique<LvalueToRvalueCast>(node->loc(), std::move(node));
    }
    return std::make_unique<IntegralCast>(node->loc(), to, std::move(node));
  }
};

/** @brief Register the WiderTypeCast conversion in the cast-rule factory. */
// NOLINTNEXTLINE
HZCC_REG_CAST_RULE(WiderTypeCast, "WiderTypeCast", "cast rule for wider type");

} // namespace hzcc::ast