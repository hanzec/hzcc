//
// Created by chen_ on 2022/6/15.
//
#include "AST/cast/expr/ImplicitCastExpr.h"
#ifndef MYCC_SOURCE_AST_CAST_EXPR_IMPLICIT_CAST_ARRAYTOPOINTERDECAYCAST_H_
#define MYCC_SOURCE_AST_CAST_EXPR_IMPLICIT_CAST_ARRAYTOPOINTERDECAYCAST_H_
namespace Hzcc::AST {
class ArrayToPointerDecayCast : public ImplicitCastExpr {
  public:
    /**
     * @brief Construct a new ArrayToPointerDecay Cast. The expression has to be
     * an rvalue otherwise it will throw an runtime error.
     * @param location The location of the cast operator.
     * @param expr The expression to cast.
     */
    ArrayToPointerDecayCast(const Position& location,        // NOLINT
                            std::unique_ptr<ASTNode> expr);  // NOLINT

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> RetType() const override;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_CAST_EXPR_IMPLICIT_CAST_ARRAYTOPOINTERDECAYCAST_H_
