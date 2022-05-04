//
// Created by chen_ on 2022/3/27.
//
#include <memory>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_CONDITIONAL_H_
#define MYCC_SOURCE_AST_OPERATOR_CONDITIONAL_H_
namespace Hzcc::AST {
class ConditionalExpr : public ASTNode {
  public:
    ConditionalExpr(std::unique_ptr<ASTNode> cond,
                    std::unique_ptr<ASTNode> true_expr,
                    std::unique_ptr<ASTNode> false_expr,
                    std::pair<int, int> location)
        : ASTNode(location),
          _cond(std::move(cond)),
          _true_expr(std::move(true_expr)),
          _false_expr(std::move(false_expr)){};

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    std::unique_ptr<ASTNode> _cond;
    std::unique_ptr<ASTNode> _true_expr;
    std::unique_ptr<ASTNode> _false_expr;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_CONDITIONAL_H_
