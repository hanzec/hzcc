//
// Created by Hanze Chen on 2022/3/25.
//
#include "AST/expr/op/OperatorBase.h"
#ifndef HZCC_AST_EXPR_OP_COMMA_EXPR_H
#define HZCC_AST_EXPR_OP_COMMA_EXPR_H
namespace Hzcc::AST {
class CommaExpr : public OperatorBase {
  public:
    /**
     * @brief CommaExpr constructor
     * @param lhs left hand side expression
     * @param rhs right hand side expression
     * @param loc location of comma operator
     */
    CommaExpr(const Position& loc, const std::string_view& type,
              std::unique_ptr<ASTNode> lhs,
              std::unique_ptr<ASTNode> rhs);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "CommaExpr"
     */
    [[nodiscard]] const char* NodeName() const override;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_OP_COMMA_EXPR_H
