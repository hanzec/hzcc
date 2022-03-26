//
// Created by chen_ on 2022/3/25.
//
#include <memory>
#include "AST/ast_node.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_SIZEOF_H_
#define MYCC_SOURCE_AST_OPERATOR_SIZEOF_H_
namespace Mycc::AST {
class SizeofExpr : public ASTNode{
  public:
    SizeofExpr(std::unique_ptr<ASTNode> expr)
        : _expr(std::move(expr)) {}


  private:
    std::unique_ptr<ASTNode> _expr;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_SIZEOF_H_
