//
// Created by chen_ on 2022/3/24.
//

#include <memory>

#include "AST/ast_node.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_RETURN_H_
#define MYCC_SOURCE_AST_STATEMENT_RETURN_H_
namespace Mycc::AST {
class ReturnNode : public ASTNode {
  public:
    ReturnNode(std::unique_ptr<ASTNode> return_val)
        : _return_val(std::move(return_val)){};
  private:
    std::unique_ptr<ASTNode> _return_val;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_RETURN_H_
