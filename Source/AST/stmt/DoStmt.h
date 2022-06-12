//
// Created by chen_ on 2022/3/25.
//
#include "WhileStmt.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_DO_H_
#define MYCC_SOURCE_AST_STATEMENT_DO_H_
namespace Hzcc::AST {
class DoStatement : public ASTNode {
  public:
    /**
     * @brief Constructor of DoStatement
     * @param cond condition of the do statement
     * @param body body of the do statement
     * @param pos location of the do statement
     */
    DoStatement(std::unique_ptr<ASTNode> cond,  // NOLINT
                std::unique_ptr<ASTNode> body,  // NOLINT
                const Position& pos);           // NOLINT

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
     * @return will always return "DoStatement"
     */
    [[nodiscard]] const char* NodeName() const override;

  protected:
    /**
     * @brief An override function using print extra information when call
     * ASTNode->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;

  protected:
    std::unique_ptr<ASTNode> _cond;
    std::unique_ptr<ASTNode> _body;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_DO_H_
