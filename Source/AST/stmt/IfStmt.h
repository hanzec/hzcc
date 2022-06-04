//
// Created by chen_ on 2022/3/24.
//
#include <vector>

#include "AST/ASTNode.h"
#ifndef HZCC_AST_STATEMENT_IF_STMT_H
#define HZCC_AST_STATEMENT_IF_STMT_H
namespace Hzcc::AST {
class IfStmt : public ASTNode {
  public:
    /**
     * @brief Constructor of IfStmt
     * @param cond condition of the if statement
     * @param body body of the if statement
     * @param location location of the if statement
     */
    IfStmt(std::unique_ptr<ASTNode> cond,         // NOLINT
           std::unique_ptr<ASTNode> body,         // NOLINT
           const std::pair<int, int>& location);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    bool setElse(std::unique_ptr<ASTNode> Else);

    [[nodiscard]] const std::unique_ptr<ASTNode>& CondStmt();

    [[nodiscard]] const std::unique_ptr<ASTNode>& BodyStmt();

    [[nodiscard]] std::unique_ptr<ASTNode>& ElseStmt();

    /**
     * @brief Get the list of else-if stmt. The return format is a vector
     * of pair, where the first element is the condition, and the second element
     * is the body.
     * @return The list of else-if stmt.
     */
    [[nodiscard]] std::vector<
        std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>>&
    ElseIfStmt();

    bool addElseIf(std::unique_ptr<ASTNode> Cond,
                   std::unique_ptr<ASTNode> Body);

    [[nodiscard]] bool HasElse() const;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool HasBody() const override;

    /**
     * @brief Get Name of the node
     * @return will always return "IfStmt"
     */
    [[nodiscard]] const char* NodeName() const override;

  protected:
    /**
     * @brief An override function using print extra information when call
     * ASTNode->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     * @return std::string generated string
     */
    [[nodiscard]] std::string PrintDetail(
        const std::string& ident) const override;

  private:
    const std::unique_ptr<ASTNode> _condition;
    const std::unique_ptr<ASTNode> _if_body_statement;
    std::unique_ptr<ASTNode> _else_statement_{nullptr};
    std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>>
        _elseIfs;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENT_IF_STMT_H
