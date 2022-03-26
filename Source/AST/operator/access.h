//
// Created by chen_ on 2022/3/25.
//
#include <memory>
#include <string>
#include <utility>

#include "AST/ast_node.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_ACCESS_H_
#define MYCC_SOURCE_AST_OPERATOR_ACCESS_H_
namespace Mycc::AST {
class AccessExpr : public ASTNode {
  public:
    AccessExpr(bool isPtr, std::string field, std::unique_ptr<ASTNode> expr)
        : _ptr_access(isPtr),
          _varaible(std::move(expr)),
          _field(std::move(field)) {}

  private:
    std::string _field;
    bool _ptr_access = false;
    std::unique_ptr<ASTNode> _varaible;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ACCESS_H_
