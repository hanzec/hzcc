//
// Created by chen_ on 2022/6/13.
//
#include "AST/type/Type.h"
#ifndef MYCC_SOURCE_AST_CAST_RULE_ICASTRULE_H_
#define MYCC_SOURCE_AST_CAST_RULE_ICASTRULE_H_
namespace Hzcc::AST {
class CastExpr;
class ICastRule {
  public:
    virtual bool CouldApplyTo(
        const std::shared_ptr<Type>& lhs,          // NOLINT
        const std::unique_ptr<ASTNode>& rhs) = 0;  // NOLINT

    virtual std::unique_ptr<CastExpr> Apply(
        std::unique_ptr<ASTNode> node, const std::shared_ptr<Type>& to) = 0;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_CAST_RULE_ICASTRULE_H_
