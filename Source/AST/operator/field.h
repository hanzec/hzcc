//
// Created by chen_ on 2022/3/25.
//
#include <string_view>
#include <memory>
#include <utility>
#include "AST/ast_node.h"
#include "AST/value/variable.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_ACCESSFIELD_H_
#define MYCC_SOURCE_AST_OPERATOR_ACCESSFIELD_H_
namespace Mycc::AST {
class FieldAccessExpr : public ASTNode{
  public:
    FieldAccessExpr(std::shared_ptr<Variable>  var, const std::string_view& field)
        : _parent(std::move(var)),_field(field) {}

  private:
    std::string_view _field;
    std::shared_ptr<Variable> _parent;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ACCESSFIELD_H_
