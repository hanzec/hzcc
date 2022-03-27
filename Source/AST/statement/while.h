//
// Created by chen_ on 2022/3/25.
//
#include <memory>

#include "AST/ast_node.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_WHILE_H_
#define MYCC_SOURCE_AST_STATEMENT_WHILE_H_
namespace Mycc::AST {

class WhileStatement : public ASTNode {
  public:
    WhileStatement(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> body)
        : cond_(std::move(cond)), body_(std::move(cond)) {}

    [[nodiscard]]  bool HasBody() const override{ return true; }

#ifdef NDEBUG
    [[nodiscard]]  std::string Dump(std::string_view ident) const override;
#endif

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  protected:
    std::unique_ptr<ASTNode> cond_;
    std::unique_ptr<ASTNode> body_;
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_WHILE_H_
