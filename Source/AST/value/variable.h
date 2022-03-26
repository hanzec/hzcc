#include <string>
#include <utility>
#include "AST/ast_node.h"

#ifndef MYCC_SOURCE_AST_VARIABLES_H
#define MYCC_SOURCE_AST_VARIABLES_H
namespace Mycc::AST {
class VariableExpr : public ASTNode{
  public:
    explicit VariableExpr(std::string name)
        : _name(std::move(name)) {}
  private:
    std::string _name{};
};
}  // namespace Mycc::AST
#endif // MYCC_SOURCE_AST_VARIABLES_H