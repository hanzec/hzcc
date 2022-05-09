//
// Created by chen_ on 2022/3/25.
//
#include <memory>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_WHILE_H_
#define MYCC_SOURCE_AST_STATEMENT_WHILE_H_
namespace Hzcc::AST {

class WhileStatement : public ASTNode {
  public:
    WhileStatement(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> body,
                   std::pair<int, int> location);

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool HasBody() const override { return true; }

    [[nodiscard]] std::unique_ptr<ASTNode>& CondStmt();

    [[nodiscard]] std::unique_ptr<ASTNode>& BodyStmt();

#ifdef NDEBUG
    [[nodiscard]] std::string Dump(const std::string& ident) const override;
#endif

    [[nodiscard]] const char* NodeName() const override;

  protected:
    [[nodiscard]] std::string PrintAdditionalInfo(
        const std::string& ident) const override;

  protected:
    std::unique_ptr<ASTNode> _cond;
    std::unique_ptr<ASTNode> _body;
};

}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_WHILE_H_
