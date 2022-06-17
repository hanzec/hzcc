//
// Created by chen_ on 2022/3/25.
//
#include "AST/ASTNode.h"
#include "AST/type/Type.h"
#ifndef HZCC_AST_EXPR_CAST_EXPR_H
#define HZCC_AST_EXPR_CAST_EXPR_H
namespace Hzcc::AST {
class CastExpr : public ASTNode {
  public:
    /**
     * @brief CastExpr constructor. Will cast expression to the given
     * type. If runtime assert is enabled, will trigger an error if the given
     * type is same as the expression's type.
     * @param type the type to cast to
     * @param expr the expression to cast
     * @param location location of the cast operator
     */
    CastExpr(const Position& location,        // NOLINT
             std::unique_ptr<ASTNode> expr);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Determine whether the node is a literal node or not
     * @return true if the node is a literal node, false otherwise
     */
    [[nodiscard]] bool IsLiteral() const override;

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or register value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override;

    /**
     * @brief return the expression that is being casted
     * @return the expression that is being casted
     */
    [[nodiscard]] const std::unique_ptr<ASTNode>& GetCastExpr() const;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "CastExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

  private:
    const std::unique_ptr<ASTNode> _cast_expr;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_CAST_EXPR_H
