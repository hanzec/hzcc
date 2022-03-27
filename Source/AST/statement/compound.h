//
// Created by chen_ on 2022/3/24.
//
#include <memory>
#include <vector>

#include "AST/ast_node.h"
#ifndef MYCC_SOURCE_AST_STATEMENT_BLOCK_H_
#define MYCC_SOURCE_AST_STATEMENT_BLOCK_H_

namespace Mycc::AST {
class CompoundStmt : public ASTNode {
  public:
    explicit CompoundStmt(std::pair<int,int> loc);

    bool AddStatement(std::unique_ptr<ASTNode> statement);

#ifdef NDEBUG
    [[nodiscard]]  std::string Dump(std::string_view ident) const override;
#endif

    const std::unique_ptr<ASTNode>& GetLastStatement() const;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;
    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    std::list<std::unique_ptr<ASTNode>> statements_{};
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_BLOCK_H_
