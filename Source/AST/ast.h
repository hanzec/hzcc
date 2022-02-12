#ifndef MYCC_AST_AST_H
#define MYCC_AST_AST_H
#include "context.h"
#include <memory>
#include <vector>
#include "expr_node.h"
#include "function_node.h"
#include "function_node.h"

namespace Mycc::AST{
class AST {
  public:
    AST();

    

  private:
    Context context_;

    std::vector<ExprNode> expr_nodes_;

};

} // namespace Mycc::AST

#endif  // MYCC_AST_AST_H