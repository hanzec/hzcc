//
// Created by chen_ on 2022/6/13.
//
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "utils/status/statusor.h"
#ifndef MYCC_SOURCE_AST_CAST_RULE_ICASTRULE_H_
#define MYCC_SOURCE_AST_CAST_RULE_ICASTRULE_H_
namespace hzcc::ast {
class ICastRule {
  public:
    virtual bool CouldApplyTo(const std::shared_ptr<Type>& lhs,   // NOLINT
                              const std::unique_ptr<Expr>& rhs);  // NOLINT

    virtual ~ICastRule() = default;

    virtual StatusOr<std::unique_ptr<CastExpr>> Apply(
        std::unique_ptr<Expr> node, const std::shared_ptr<Type>& to);

    ICastRule() = default;
};

inline bool ICastRule::CouldApplyTo(
    const std::shared_ptr<Type>& lhs,    // NOLINT
    const std::unique_ptr<Expr>& rhs) {  // NOLINT
    return false;
}

inline StatusOr<std::unique_ptr<CastExpr>> ICastRule::Apply(
    std::unique_ptr<Expr> node, const std::shared_ptr<Type>& to) {
    return Status(StatusCode::kUnimplemented, "Not implemented");
}
}  // namespace hzcc::ast
#endif  // MYCC_SOURCE_AST_CAST_RULE_ICASTRULE_H_
