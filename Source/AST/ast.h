#include <memory>
#include <unordered_map>
#include <vector>
#include "expr_node.h"
#include "function_node.h"
#include "function_node.h"

#ifndef MYCC_AST_AST_H
#define MYCC_AST_AST_H

namespace Mycc::AST{
class ASTContext {
  public:
    ASTContext();

    
  
  private:
    std::vector<std::unique_ptr<ExprNode>> _expr_nodes;

    std::unordered_map<std::string,ExprNode> _function_map;

};

} // namespace Mycc::AST

#endif  // MYCC_AST_AST_H