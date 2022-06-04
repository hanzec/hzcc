//
// Created by chen_ on 2022/3/24.
//

#include "AST/ASTNode.h"
#ifndef HZCC_AST_STATEMENT_RETURN_STMT_H
#define HZCC_AST_STATEMENT_RETURN_STMT_H
namespace Hzcc::AST {
class ReturnStmt : public ASTNode {
  public:
    /**
     * @brief Constructor of ReturnStmt
     * @param expr expression of the return statement
     * @param loc location of the return statement
     */
    ReturnStmt(std::unique_ptr<ASTNode> expr,  // NOLINT
               const Position& loc);           // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Get Name of the node
     * @return will always return "ReturnStmt"
     */
    [[nodiscard]] const char* NodeName() const override;

    /**
     * @brief Get the returned expression
     * @return The returned expression
     */
    std::unique_ptr<ASTNode>& GetReturnVal();

    /**
     * @brief Determine whether the node is a return statement or not
     * @return true if the node is a return statement, false otherwise
     */
    [[nodiscard]] bool IsReturn() const override { return true; }

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
    std::unique_ptr<ASTNode> _return_val;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENT_RETURN_STMT_H
