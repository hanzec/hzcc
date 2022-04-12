//
// Created by chen_ on 2022/3/25.
//
#include <memory>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_SIZEOF_H_
#define MYCC_SOURCE_AST_OPERATOR_SIZEOF_H_
namespace Mycc::AST {
class SizeofExpr : public ASTNode {
  public:
    SizeofExpr(std::unique_ptr<ASTNode> expr, std::pair<int, int> location)
        : ASTNode(location), _expr(std::move(expr)) {}

    void visit(ASTVisitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    std::unique_ptr<ASTNode> _expr;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_SIZEOF_H_
