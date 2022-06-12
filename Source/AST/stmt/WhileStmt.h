//
// Created by chen_ on 2022/3/25.
//

#include "AST/ASTNode.h"

#ifndef HZCC_AST_STATEMENT_WHILE_H
#define HZCC_AST_STATEMENT_WHILE_H
namespace Hzcc::AST {

class WhileStmt : public ASTNode {
  public:
    /**
     * @brief Constructor of WhileStmt
     * @param cond condition of the while
     * @param body body of the while
     * @param loc location of the while
     */
    WhileStmt(const Position& loc, std::unique_ptr<ASTNode> cond,
              std::unique_ptr<ASTNode> body);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool HasBody() const override { return true; }

    [[nodiscard]] std::unique_ptr<ASTNode>& CondStmt();

    [[nodiscard]] std::unique_ptr<ASTNode>& BodyStmt();

    /**
     * @brief Get Name of the node
     * @return will always return "WhileStmt"
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
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;

  protected:
    std::unique_ptr<ASTNode> _cond;
    std::unique_ptr<ASTNode> _body;
};

}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENT_WHILE_H
