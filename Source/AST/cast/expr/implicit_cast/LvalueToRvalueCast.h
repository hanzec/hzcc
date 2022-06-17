//
// Created by chen_ on 2022/6/15.
//
#include "AST/cast/expr/ImplicitCastExpr.h"
#ifndef MYCC_SOURCE_AST_CAST_EXPR_IMPLICT_CAST_LVALUETORVALUE_H_
#define MYCC_SOURCE_AST_CAST_EXPR_IMPLICT_CAST_LVALUETORVALUE_H_
namespace Hzcc::AST {
class LvalueToRvalueCast : public ImplicitCastExpr {
  public:
    /**
     * @brief Construct a new LvalueToRvalue Cast. The expression has to be an
     * rvalue otherwise it will throw an runtime error.
     * @param location The location of the cast operator.
     * @param expr The expression to cast.
     */
    LvalueToRvalueCast(const Position& location,        // NOLINT
                       std::unique_ptr<ASTNode> expr);  // NOLINT

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or register value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override;

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> RetType() const override;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_CAST_EXPR_IMPLICT_CAST_LVALUETORVALUE_H_
