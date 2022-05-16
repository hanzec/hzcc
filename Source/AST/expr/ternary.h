//
// Created by chen_ on 2022/3/27.
//
#include <memory>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_CONDITIONAL_H_
#define MYCC_SOURCE_AST_OPERATOR_CONDITIONAL_H_
namespace Hzcc::AST {
class TernaryExpr : public ASTNode {
  public:
    TernaryExpr(std::unique_ptr<ASTNode> cond,
                std::unique_ptr<ASTNode> true_expr,
                std::unique_ptr<ASTNode> false_expr,
                std::pair<int, int> location)
        : ASTNode(location),
          _cond(std::move(cond)),
          _true_expr(std::move(true_expr)),
          _false_expr(std::move(false_expr)){};

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    [[nodiscard]] std::unique_ptr<ASTNode>& GetContStmt();

    [[nodiscard]] std::unique_ptr<ASTNode>& GetTrueExpr();

    [[nodiscard]] std::unique_ptr<ASTNode>& GetFalseExpr();

    [[nodiscard]] const char* NodeName() const override;

  protected:
    [[nodiscard]] std::string PrintAdditionalInfo(
        const std::string& ident) const override;

  private:
    std::unique_ptr<ASTNode> _cond;
    std::unique_ptr<ASTNode> _true_expr;
    std::unique_ptr<ASTNode> _false_expr;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_CONDITIONAL_H_