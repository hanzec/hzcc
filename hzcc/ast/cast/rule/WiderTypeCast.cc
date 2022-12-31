//
// Created by chen_ on 2022/6/13.
//
#include "ast/cast/Cast.h"
#include "ast/cast/ICastRule.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include <magic_enum.hpp>

namespace hzcc::ast {
/**
 * @brief WiderTypeCast Cast Rule
 * @details This Rule to cast a type to a wider type (e.g. int to long). The
 * WiderTypeCast only support built-in types. And converting rules are using the
 * following rules (rows converted to columns):
 *      |             | short | int | long | long long | char | void | float | double | long double |
 *      | :---------: | :---: | :-: | :--: | :-------: | :--: | :--: | :---: | :----: | :---------: |
 *      |    short    |  NA   | Yes |  Yes |    Yes    |  No  |  No  |  Yes  |  Yes   |     Yes     |
 *      |     int     |  No   | NA  |  Yes |    Yes    |  No  |  No  |  Yes  |  Yes   |     Yes     |
 *      |    long     |  No   | No  |  NA  |    Yes    |  No  |  No  |  Yes  |  Yes   |     Yes     |
 *      |  long long  |  No   | No  |  No  |    NA     |  No  |  No  |  No   |  Yes   |     Yes     |
 *      |    char     |  Yes  | Yes |  Yes |    Yes    |  NA  |  No  |  Yes  |  Yes   |     Yes     |
 *      |    void     |  No   | No  |  No  |    No     |  No  |  No  |  No   |  No    |     No      |
 *      |    float    |  No   | No  |  No  |    No     |  No  |  No  |  NA   |  Yes   |    Yes      |
 *      |   double    |  No   | No  |  No  |    No     |  No  |  No  |  No   |  NA    |    Yes      |
 *      | long double |  No   | No  |  No  |    No     |  No  |  No  |  No   |  No    |    NA       |
 */
class WiderTypeCast : public ICastRule {
public:
  bool CouldApplyTo(const std::shared_ptr<Type> &lhs,            // NOLINT
                    const std::unique_ptr<Expr> &rhs) override { // NOLINT
    auto rhs_type = rhs->retType();

    // builtin type only
    if (lhs->IsNumericalType() && rhs_type->IsNumericalType()) {
      auto lhs_builtin = std::dynamic_pointer_cast<NumericalType>(lhs);
      auto rhs_builtin = std::dynamic_pointer_cast<NumericalType>(rhs_type);
      return kCastTable[lhs_builtin->GetTypeId()][rhs_builtin->GetTypeId()];
    } else {
      return false;
    }
  }

  StatusOr<std::unique_ptr<CastExpr>> Apply(std::unique_ptr<Expr> node, // NOLINT
                                            const std::shared_ptr<Type> &to) override {
    auto rhs_type = std::dynamic_pointer_cast<NumericalType>(node->retType());
    if (rhs_type->GetTypeId() < magic_enum::enum_integer(PrimitiveType::kFloat)) {
      return std::make_unique<IntegralCast>(node->loc(), to, std::move(node));
    } else {
      LOG(WARNING) << "WiderTypeCast: " << rhs_type->Name() << " to " << to->Name() << " is not impl.";
      return nullptr;
    }
  }

private:
  constexpr static std::array<std::array<bool, magic_enum::enum_count<PrimitiveType>()>,
                              magic_enum::enum_count<PrimitiveType>()>
      kCastTable{{{{false, true, true, true, false, false, true, true, true}},         // short
                  {{false, false, true, true, false, false, true, true, true}},        // int
                  {{false, false, false, true, false, false, true, true, true}},       // long
                  {{false, false, false, false, false, false, false, true, true}},     // longlong
                  {{true, true, true, true, false, false, true, true, true}},          // char
                  {{false, false, false, false, false, false, false, false, false}},   // void
                  {{false, false, false, false, false, false, false, true, true}},     // float
                  {{false, false, false, false, false, false, false, false, true}},    // double
                  {{false, false, false, false, false, false, false, false, false}}}}; // long double
};

// Register pass
// NOLINTNEXTLINE
HZCC_REG_CAST_RULE(WiderTypeCast, "WiderTypeCast", "cast rule for wider type");

} // namespace hzcc::ast