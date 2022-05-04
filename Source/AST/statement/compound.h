//
// Created by chen_ on 2022/3/24.
//
#ifndef MYCC_SOURCE_AST_STATEMENT_BLOCK_H_
#define MYCC_SOURCE_AST_STATEMENT_BLOCK_H_
#include <list>
#include <memory>
#include <vector>

#include "AST/ASTNode.h"
namespace Hzcc::AST {
class CompoundStmt : public ASTNode {
  public:
    explicit CompoundStmt(std::pair<int, int> loc);

    Status visit(ASTVisitor& visitor) override;

    bool AddStatement(std::unique_ptr<ASTNode> statement);

#ifdef NDEBUG
    [[nodiscard]] std::string Dump(std::string_view ident) const override;
#endif

    [[nodiscard]] const std::unique_ptr<ASTNode>& GetLastStatement() const;

    [[nodiscard]] std::list<std::unique_ptr<ASTNode>>& GetBodyStatements();

  protected:
    [[nodiscard]] const char* GetNodeName() const override;
    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    std::list<std::unique_ptr<ASTNode>> statements_{};
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_BLOCK_H_
