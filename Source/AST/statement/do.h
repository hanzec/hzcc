//
// Created by chen_ on 2022/3/25.
//
#include "while.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_DO_H_
#define MYCC_SOURCE_AST_STATEMENT_DO_H_
namespace Mycc::AST {
class DoStatement : public WhileStatement {
  public:
    DoStatement(std::unique_ptr<AST::ASTNode> cond,
                std::unique_ptr<AST::ASTNode> body, std::pair<int, int> pos)
        : WhileStatement(std::move(cond), std::move(body), pos) {}

    void visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool HasBody() const override { return true; }

  protected:
    [[nodiscard]] std::string GetNodeName() const override;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_DO_H_
