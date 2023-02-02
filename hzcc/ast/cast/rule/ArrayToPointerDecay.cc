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
class ArrayToPointerDecay : public ICastRule {
public:
  bool CouldApplyTo(const ExprPtr &rhs, const QualTypePtr &lhs) override {
    /**
     * In order to apply this rule, the rhs has to be an ArraySubscriptExpr
     * ,the rhs has to be a pointer type and lhs has to be not a pointer
     */
    return !lhs->is<TypeCategory::Ptr>() && rhs->type()->is<TypeCategory::Ptr>() &&
           (strcmp(rhs->NodeName().data(), "ArraySubscriptExpr") == 0);
  }

  StatusOr<std::unique_ptr<CastExpr>> Apply(std::unique_ptr<Expr> node, const QualTypePtr &to) override {
    return std::make_unique<ArrayToPointerDecayCast>(node->loc(), std::move(node));
  }
};

// Register pass
HZCC_REG_CAST_RULE(ArrayToPointerDecay, "ArrayToPointerDecay", "ArrayToPointerDecayCast")

} // namespace hzcc::ast