//
// Created by chen_ on 2022/3/27.
//
#include "AST/ASTNode.h"
#ifndef HZCC_AST_EXPR_TERNARY_H
#define HZCC_AST_EXPR_TERNARY_H
namespace Hzcc::AST {
class TernaryExpr : public ASTNode {
  public:
    TernaryExpr(const Position& location, std::unique_ptr<ASTNode> cond,
                std::unique_ptr<ASTNode> true_expr,
                std::unique_ptr<ASTNode> false_expr);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

    [[nodiscard]] std::unique_ptr<ASTNode>& GetContStmt();

    [[nodiscard]] std::unique_ptr<ASTNode>& GetTrueExpr();

    [[nodiscard]] std::unique_ptr<ASTNode>& GetFalseExpr();

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "TernaryExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

    /**
     * @brief An override function using print extra information when call
     * ASTNode->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     * @return std::string generated string
     */
    [[nodiscard]] std::string PrintDetail(
        const std::string& ident) const override;

  private:
    std::unique_ptr<ASTNode> _cond;
    std::unique_ptr<ASTNode> _true_expr;
    std::unique_ptr<ASTNode> _false_expr;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_TERNARY_H
