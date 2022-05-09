//
// Created by chen_ on 2022/3/25.
//
#include <memory>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_FOR_H_
#define MYCC_SOURCE_AST_STATEMENT_FOR_H_
namespace Hzcc::AST {
class ForStatement : public ASTNode {
  public:
    ForStatement(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> cond,
                 std::unique_ptr<ASTNode> step, std::unique_ptr<ASTNode> body,
                 std::pair<int, int> location);

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool HasBody() const override { return true; }

#ifdef NDEBUG
    [[nodiscard]] std::string Dump(const std::string& ident) const override;
#endif

    /**
     * @brief Get the init statement.
     * @return The init statement.
     */
    [[nodiscard]] std::unique_ptr<ASTNode>& InitStmt();

    /**
     * @brief Get the condition statement.
     * @return The condition statement.
     */
    [[nodiscard]] std::unique_ptr<ASTNode>& CondStmt();

    /**
     * @brief Get the step expression.
     * @return  The step expression.
     */
    [[nodiscard]] std::unique_ptr<ASTNode>& StepStmt();

    /**
     * @brief Get the body of the for statement
     * @return  The body of the for statement
     */
    [[nodiscard]] std::unique_ptr<ASTNode>& BodyStmt();

    [[nodiscard]] const char* NodeName() const override;

  protected:
    [[nodiscard]] std::string PrintAdditionalInfo(
        const std::string& ident) const override;

  private:
    std::unique_ptr<ASTNode> _init;
    std::unique_ptr<ASTNode> _cond;
    std::unique_ptr<ASTNode> _step;
    std::unique_ptr<ASTNode> _body;
};

};      // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_FOR_H_
