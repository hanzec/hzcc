//
// Created by chen_ on 2022/3/25.
//
#include "while.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_DO_H_
#define MYCC_SOURCE_AST_STATEMENT_DO_H_
namespace Hzcc::AST {
class DoStatement : public ASTNode {
  public:
    DoStatement(std::unique_ptr<AST::ASTNode> cond,
                std::unique_ptr<AST::ASTNode> body, std::pair<int, int> pos)
        : ASTNode(pos), _cond(std::move(cond)), _body(std::move(body)) {
        DLOG_ASSERT(_cond != nullptr)
            << " condition is nullptr for do statement (id: " << GetNodeId()
            << ")";
        DLOG_ASSERT(_body != nullptr)
            << " body is nullptr for do statement (id: " << GetNodeId() << ")";
    }

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool HasBody() const override { return true; }

    [[nodiscard]] const char* NodeName() const override;

    [[nodiscard]] std::unique_ptr<ASTNode>& CondStmt();

    [[nodiscard]] std::unique_ptr<ASTNode>& BodyStmt();

  protected:
    [[nodiscard]] std::string PrintAdditionalInfo(
        const std::string& ident) const override;

  protected:
    std::unique_ptr<ASTNode> _cond;
    std::unique_ptr<ASTNode> _body;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_DO_H_
