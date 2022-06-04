//
// Created by chen_ on 2022/3/25.
//
#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_FOR_H_
#define MYCC_SOURCE_AST_STATEMENT_FOR_H_
namespace Hzcc::AST {
class ForStmt : public ASTNode {
  public:
    /**
     * @brief Constructor of ForStmt
     * @param init initial expression of the for loop
     * @param cond condition of the for loop
     * @param step step expression of the for loop
     * @param body body of the for loop
     * @param location location of the for loop
     */
    ForStmt(const Position& location, std::unique_ptr<ASTNode> init,
            std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> step,
            std::unique_ptr<ASTNode> body);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Get Name of the node
     * @return will always return "ForStmt"
     */
    [[nodiscard]] const char* NodeName() const override;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool HasBody() const override { return true; }

    /**
     * @brief Get the init stmt.
     * @return The init stmt.
     */
    [[nodiscard]] const std::unique_ptr<ASTNode>& InitStmt();

    /**
     * @brief Get the condition stmt.
     * @return The condition stmt.
     */
    [[nodiscard]] const std::unique_ptr<ASTNode>& CondStmt();

    /**
     * @brief Get the step expression.
     * @return  The step expression.
     */
    [[nodiscard]] const std::unique_ptr<ASTNode>& StepStmt();

    /**
     * @brief Get the body of the for stmt
     * @return  The body of the for stmt
     */
    [[nodiscard]] const std::unique_ptr<ASTNode>& BodyStmt();

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
    const std::unique_ptr<ASTNode> _init;
    const std::unique_ptr<ASTNode> _cond;
    const std::unique_ptr<ASTNode> _step;
    const std::unique_ptr<ASTNode> _body;
};

};      // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_FOR_H_
