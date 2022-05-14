//
// Created by chen_ on 2022/3/24.
//
#include <memory>
#include <vector>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_IF_H_
#define MYCC_SOURCE_AST_STATEMENT_IF_H_
namespace Hzcc::AST {
class IfStatement : public ASTNode {
  public:
    IfStatement(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> body,
                std::pair<int, int> location)
        : ASTNode(location),
          _condition(std::move(cond)),
          _if_body_statement(std::move(body)){};

    Status visit(ASTVisitor& visitor) override;

    bool setElse(std::unique_ptr<ASTNode> Else);

    [[nodiscard]] std::unique_ptr<ASTNode>& CondStmt();

    [[nodiscard]] std::unique_ptr<ASTNode>& BodyStmt();

    [[nodiscard]] std::unique_ptr<ASTNode>& ElseStmt();

    /**
     * @brief Get the list of else-if statement. The return format is a vector
     * of pair, where the first element is the condition, and the second element
     * is the body.
     * @return The list of else-if statement.
     */
    [[nodiscard]] std::vector<
        std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>>&
    ElseIfStmt();

    bool addElseIf(std::unique_ptr<ASTNode> Cond,
                   std::unique_ptr<ASTNode> Body);

    [[nodiscard]] bool HasElse() const;

    [[nodiscard]] bool HasBody() const override;

#ifdef NDEBUG
    [[nodiscard]] std::string Dump(const std::string& ident) const override;
#endif

    [[nodiscard]] const char* NodeName() const override;

  protected:
    [[nodiscard]] std::string PrintAdditionalInfo(
        const std::string& ident) const override;

  private:
    std::unique_ptr<ASTNode> _condition;
    std::unique_ptr<ASTNode> _if_body_statement;
    std::unique_ptr<ASTNode> _else_statement_{nullptr};
    std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>>
        _elseIfs;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_IF_H_
