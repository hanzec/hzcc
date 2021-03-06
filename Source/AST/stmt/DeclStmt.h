//
// Created by chen_ on 2022/3/28.
//
#include "AST/ASTNode.h"
#ifndef HZCC_AST_STATEMENT_DECL_STMT_H
#define HZCC_AST_STATEMENT_DECL_STMT_H
namespace Hzcc::AST {
class DeclStmt : public ASTNode {
  public:
    /**
     * @brief Constructor of DeclStmt
     * @param name name of the declaration
     * @param loc location of the declaration
     */
    DeclStmt(std::shared_ptr<Type> type,    // NOLINT
             const std::string_view& name,  // NOLINT
             const Position& loc);          // NOLINT

    [[nodiscard]] std::string GetName() const;

    /**
     * @brief Determine whether the current AST node is a declaration node or
     * not
     * @return true if the node is a declaration node, false otherwise
     */
    [[nodiscard]] bool IsDeclNode() const override;

    [[nodiscard]] virtual bool IsFuncDecl() const;

    [[nodiscard]] virtual bool HasInitExpr() const;

    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

    /**
     * @brief Get Name of the node
     * @return will always return "DeclStmt"
     */
    [[nodiscard]] const char* NodeName() const override;

  private:
    const std::string _decl_name;
    const std::shared_ptr<Type> _type;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENT_DECL_STMT_H
