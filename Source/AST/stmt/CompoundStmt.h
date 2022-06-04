//
// Created by chen_ on 2022/3/24.
//
#include <list>

#include "AST/ASTNode.h"
#ifndef HZCC_AST_STATEMENT_COMPOUND_STMT_H
#define HZCC_AST_STATEMENT_COMPOUND_STMT_H
namespace Hzcc::AST {
class CompoundStmt : public ASTNode {
  public:
    /**
     * @brief Constructor of CompoundStmt
     * @param loc location of the compound statement
     */
    explicit CompoundStmt(const Position& loc);

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Get Name of the node
     * @return will always return "CompoundStmt"
     */
    [[nodiscard]] const char* NodeName() const override;

    bool AddStatement(std::unique_ptr<ASTNode> statement);

    [[nodiscard]] const std::unique_ptr<ASTNode>& GetLastStatement() const;

    [[nodiscard]] std::list<std::unique_ptr<ASTNode>>& GetBodyStatements();

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
    std::list<std::unique_ptr<ASTNode>> statements_{};
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENT_COMPOUND_STMT_H
