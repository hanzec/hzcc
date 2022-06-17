//
// Created by chen_ on 2022/6/13.
//
#include "AST/ASTNode.h"
#include "AST/cast/CastApplier.h"
#include "AST/cast/ICastRule.h"
#include "AST/cast/expr/implicit_cast/ArrayToPointerDecayCast.h"
namespace Hzcc::AST {
class ArrayToPointerDecay : public ICastRule {
public:
  bool CouldApplyTo(const std::shared_ptr<Type> &lhs, // NOLINT
                    const std::unique_ptr<ASTNode> &rhs) override {
    /**
     * In order to apply this rule, the rhs has to be an ArraySubscriptExpr
     * ,the rhs has to be a pointer type and lhs has to be not a pointer
     */
    return !lhs->IsPointer() && rhs->RetType()->IsPointer() &&
           (strcmp(rhs->NodeName(), "ArraySubscriptExpr") == 0);
  }

  std::unique_ptr<CastExpr> Apply(std::unique_ptr<ASTNode> node, const std::shared_ptr<Type> &to) override {
    return std::make_unique<ArrayToPointerDecayCast>(node->Location(), std::move(node));
  }
};

// Register pass
// NOLINTNEXTLINE
static CastRuleRegister<ArrayToPointerDecay> wider_type_cast("WiderTypeCast", "cast rule for wider type");
} // namespace Hzcc::AST