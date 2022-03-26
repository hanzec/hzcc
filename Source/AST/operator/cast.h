//
// Created by chen_ on 2022/3/25.
//
#include <memory>
#include <utility>
#include "AST/ast_node.h"
#include "AST/type/type.h"
#ifndef MYCC_SOURCE_AST_OPERATOR_CAST_H_
#define MYCC_SOURCE_AST_OPERATOR_CAST_H_
namespace Mycc::AST {
class CastExpr : public ASTNode{
  public:
    explicit CastExpr(std::shared_ptr<Type> type)
        : _cast_type(std::move(type)){}

  private:
    std::shared_ptr<Type> _cast_type;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_CAST_H_
