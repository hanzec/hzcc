//
// Created by chen_ on 2022/3/25.
//
#include <memory>
#include "AST/ast_node.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_OPERATOR_H_
#define MYCC_SOURCE_AST_OPERATOR_OPERATOR_H_
namespace Mycc::AST {
class OperatorBase : public ASTNode {
  public:
    OperatorBase(std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : _lhs(std::move(lhs)), _rhs(std::move(rhs)) {}

  private:
    std::unique_ptr<ASTNode> _lhs;
    std::unique_ptr<ASTNode> _rhs;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_OPERATOR_H_
