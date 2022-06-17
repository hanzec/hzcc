//
// Created by chen_ on 2022/6/15.
//
#include "AST/cast/expr/ImplicitCastExpr.h"
#ifndef MYCC_SOURCE_AST_CAST_EXPR_IMPLICIT_CAST_INTEGRALCAST_H_
#define MYCC_SOURCE_AST_CAST_EXPR_IMPLICIT_CAST_INTEGRALCAST_H_
namespace Hzcc::AST {
class IntegralCast : public ImplicitCastExpr {
  public:
    /**
     * @brief Construct a new Integral Cast.
     * @param location The location of the cast operator.
     * @param expr The expression to cast.
     * @param type The type to cast to.
     */
    IntegralCast(const Position& location,        // NOLINT
                 std::shared_ptr<Type> type,      // NOLINT
                 std::unique_ptr<ASTNode> expr);  // NOLINT

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

  private:
    std::shared_ptr<Type> _type;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_CAST_EXPR_IMPLICIT_CAST_INTEGRALCAST_H_
