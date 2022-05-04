//
// Created by chen_ on 2022/3/25.
//
#include <memory>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_OPERATOR_H_
#define MYCC_SOURCE_AST_OPERATOR_OPERATOR_H_
namespace Hzcc::AST {
class OperatorBase : public ASTNode {
  public:
    OperatorBase(std::pair<int, int> loc, std::unique_ptr<ASTNode> lhs,
                 std::unique_ptr<ASTNode> rhs);

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    [[nodiscard]] const std::unique_ptr<ASTNode>& GetLHS() const;
    [[nodiscard]] const std::unique_ptr<ASTNode>& GetRHS() const;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    std::unique_ptr<ASTNode> _lhs;
    std::unique_ptr<ASTNode> _rhs;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_OPERATOR_H_
