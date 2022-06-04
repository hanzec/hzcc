//
// Created by Hanze Chen on 2022/3/25.
//
#include "AST/ASTNode.h"
#ifndef HZCC_AST_EXPR_OP_OPERATOR_BASE_H
#define HZCC_AST_EXPR_OP_OPERATOR_BASE_H
namespace Hzcc::AST {
class OperatorBase : public ASTNode {
  public:
    /**
     * @brief OperatorBase constructor
     * @param lhs left hand side expression
     * @param rhs right hand side expression
     * @param loc location of operator
     */
    OperatorBase(const Position& loc, std::unique_ptr<ASTNode> lhs,
                 std::unique_ptr<ASTNode> rhs);  // NOLINT

    /**
     * @brief Get the type of the current expression. The type of OperatorBase
     * and its inherited class will determine by LHS's and RHS's deduced type:
     *  1. If both LHS and RHS have the deduced type, then the final type will
     *  be same as LHS
     *  2. If only LHS has the deduced type or only RHS has the deduced type,
     *  then the returned type will be same as the expression that can not be
     *  deduced.
     * @return return the type of the current expression
     */
    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

    /**
     * @brief Get left hand side expression
     * @return left hand side expression
     */
    [[nodiscard]] const std::unique_ptr<ASTNode>& GetLHS() const;

    /**
     * @brief Get the right hand side expression
     * @return right hand side expression
     */
    [[nodiscard]] const std::unique_ptr<ASTNode>& GetRHS() const;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "OperatorBase"
     */
    [[nodiscard]] const char* NodeName() const override;

  private:
    std::unique_ptr<ASTNode> _lhs;
    std::unique_ptr<ASTNode> _rhs;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_OP_OPERATOR_BASE_H
