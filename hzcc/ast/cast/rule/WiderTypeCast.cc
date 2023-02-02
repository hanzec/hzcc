//
// Created by chen_ on 2022/6/13.
//
#include <array>
#include <exception>
#include <glog/logging.h>
#include <magic_enum.hpp>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>

#include "ast/Stmt.h"
#include "ast/cast/Cast.h"
#include "ast/cast/ICastRule.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "enums.h"
#include "utils/factory.h"
#include "utils/status/statusor.h"

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
  bool CouldApplyTo(const ExprPtr &rhs, const QualTypePtr &lhs) override { // NOLINT
    auto rhs_type = rhs->type();

    // builtin type only
    if (lhs->is<TypeCategory::Numerical>() && rhs_type->is<TypeCategory::Numerical>()) {
      auto lhs_builtin = lhs->as<NumericalType>();
      auto rhs_builtin = rhs_type->as<NumericalType>();
      return kCastTable[lhs_builtin->GetTypeId()][rhs_builtin->GetTypeId()];
    } else {
      return false;
    }
  }

  StatusOr<std::unique_ptr<CastExpr>> Apply(ExprPtr node,                     // NOLINT
                                            const QualTypePtr &to) override { // NOLINT
    auto rhs_type = node->type()->as<NumericalType>();
    if (rhs_type->GetTypeId() < magic_enum::enum_integer(PrimitiveType::kFloat)) {
      return std::make_unique<IntegralCast>(node->loc(), to, std::move(node));
    } else {
      LOG(WARNING) << "WiderTypeCast: " << rhs_type->to_str() << " to " << to->to_str() << " is not impl.";
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