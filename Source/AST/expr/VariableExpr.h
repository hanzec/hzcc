#ifndef MYCC_VARIABLE_EXPR_NODE_H
#define MYCC_VARIABLE_EXPR_NODE_H
#include "../expr_node.h"
#include <string>

namespace Mycc::AST{
class VariableExpr: public ExprNode{
  public:
  virtual ~VariableExpr() = default;
  virtual T accept(ExprNode<T>) = 0;
  private:
  std::string _name;
};
} // namespace Mycc::AST

#endif  // MYCC_VARIABLE_EXPR_NODE_H