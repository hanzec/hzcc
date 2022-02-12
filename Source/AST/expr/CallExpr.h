
#ifndef MYCC_CALL_EXPR_NODE_H
#define MYCC_CALL_EXPR_NODE_H
#include "../expr_node.h"

namespace Mycc::AST{
template<typename T>
class CallExpr: public ExprNode<T> {
  public:
  virtual ~CallExpr() = default;
  virtual T accept(ExprNode<T>) = 0;
};
} // namespace Mycc::AST

#endif  // MYCC_CALL_EXPR_NODE_H