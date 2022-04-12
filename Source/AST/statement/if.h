//
// Created by chen_ on 2022/3/24.
//
#include <memory>
#include <vector>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_IF_H_
#define MYCC_SOURCE_AST_STATEMENT_IF_H_
namespace Mycc::AST {
class IfStatement : public ASTNode {
  public:
    IfStatement(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> body,
                std::pair<int, int> location)
        : ASTNode(location),
          _condition(std::move(cond)),
          _if_body_statement(std::move(body)){};

    void visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool hasElse() const { return _else_statement_ != nullptr; }

    bool setElse(std::unique_ptr<ASTNode> Else) {
        if ((_else_statement_ = std::move(Else)) == nullptr)
            return false;
        else
            return true;
    }

    bool addElseIf(std::unique_ptr<ASTNode> Cond,
                   std::unique_ptr<ASTNode> Body) {
        if (Cond == nullptr || Body == nullptr)
            return false;
        else {
            _elseIfs.emplace_back(std::move(Cond), std::move(Body));
            return true;
        }
    }

    [[nodiscard]] bool HasBody() const override { return true; }

#ifdef NDEBUG
    [[nodiscard]] std::string Dump(std::string_view ident) const override;
#endif

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    std::unique_ptr<ASTNode> _condition;
    std::unique_ptr<ASTNode> _if_body_statement;
    std::unique_ptr<ASTNode> _else_statement_{nullptr};
    std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>>
        _elseIfs;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_IF_H_
