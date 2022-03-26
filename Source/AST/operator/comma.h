//
// Created by chen_ on 2022/3/25.
//
#include <memory>

#include "AST/ast_node.h"
#include "operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_COMMA_H_
#define MYCC_SOURCE_AST_OPERATOR_COMMA_H_
namespace Mycc::AST {
class CommaExpr : public OperatorBase {
  public:
    CommaExpr(std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : OperatorBase(std::move(rhs),std::move(lhs)){}
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_COMMA_H_
