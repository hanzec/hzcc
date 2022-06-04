//
// Created by chen_ on 2022/3/25.
//
#include "AST/ASTNode.h"
#ifndef HZCC_AST_STATEMENT_BREAK_STMT_H
#define HZCC_AST_STATEMENT_BREAK_STMT_H
namespace Hzcc::AST {
class BreakStmt : public ASTNode {
  public:
    /**
     * @brief Constructor of BreakStmt
     * @param loc location of the break statement
     */
    explicit BreakStmt(const std::pair<int, int>& loc);

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Get Name of the node
     * @return will always return "BreakStmt"
     */
    [[nodiscard]] const char* NodeName() const override;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENT_BREAK_STMT_H
