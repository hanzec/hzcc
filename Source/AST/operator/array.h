//
// Created by chen_ on 2022/3/25.
//
#include <string>
#include <utility>

#include "AST/ast_node.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_ARRAY_H_
#define MYCC_SOURCE_AST_OPERATOR_ARRAY_H_
namespace Mycc::AST {
class ArrayExpr : public ASTNode {
  public:
    ArrayExpr(std::string name, std::unique_ptr<AST::ASTNode> index)
        : _index_expr(std::move(index)), _name(std::move(name)){};

  private:
    std::string _name;
    std::unique_ptr<AST::ASTNode> _index_expr;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ARRAY_H_
