//
// Created by chen_ on 2022/3/24.
//
#include <memory>
#include <vector>
#include "AST/ast_node.h"
#ifndef MYCC_SOURCE_AST_STATEMENT_BLOCK_H_
#define MYCC_SOURCE_AST_STATEMENT_BLOCK_H_

namespace Mycc::AST {
class BlockStatement : public ASTNode {
  public:
    BlockStatement() = default;

    void AddStatement(std::unique_ptr<ASTNode> statement) {
        statements_.push_back(std::move(statement));
    }
    std::vector<std::unique_ptr<ASTNode>> statements_{};
};
} // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_BLOCK_H_
