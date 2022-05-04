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
                   std::pair<int, int> location)
        : ASTNode(location), cond_(std::move(cond)), body_(std::move(cond)) {}

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool HasBody() const override { return true; }

#ifdef NDEBUG
    [[nodiscard]] std::string Dump(std::string_view ident) const override;
#endif

  protected:
    [[nodiscard]] const char* GetNodeName() const override;

  protected:
    std::unique_ptr<ASTNode> cond_;
    std::unique_ptr<ASTNode> body_;
};

}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_WHILE_H_
