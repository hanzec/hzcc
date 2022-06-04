//
// Created by chen_ on 2022/3/25.
//
#include "AST/ASTNode.h"
#ifndef HZCC_AST_EXPR_SIZEOF_EXPR_H
#define HZCC_AST_EXPR_SIZEOF_EXPR_H
namespace Hzcc::AST {
class SizeofExpr : public ASTNode {
  public:
    /**
     * @brief Construct a new Sizeof Expr object
     * @param expr the expression to get the size of
     * @param location location of the sizeof operator
     */
    SizeofExpr(std::unique_ptr<ASTNode> expr,
               const std::pair<int, int>& location);

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Return type of the SizeofExpr
     * @return will always return "int"
     */
    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "SizeofExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

  private:
    std::unique_ptr<ASTNode> _expr;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_SIZEOF_EXPR_H
