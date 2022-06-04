//
// Created by chen_ on 2022/3/25.
//
#include "AST/ASTNode.h"
#ifndef HZCC_AST_EXPR_ACCESS_EXPR_H
#define HZCC_AST_EXPR_ACCESS_EXPR_H
namespace Hzcc::AST {
class AccessExpr : public ASTNode {
  public:
    /**
     * @brief Construct a new AccessExpr object. The AccessExpr is used to
     * access the struct member. Runtime will check the type is struct or not if
     * enabled. Also the field name must be not empty.
     * @param isPtr whether the access expression is a pointer access
     * @param field the field name
     * @param chain_access if have chain access or EmptyStmt otherwise
     * @param location location of access expression
     */
    AccessExpr(bool isPtr, const Position& location,
               const std::string_view& field,
               std::unique_ptr<ASTNode> chain_access);  // NO_LINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or register value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override;

    /**
     * @brief Get the type of specified field.
     * @return he type of specified field.
     */
    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "AccessExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

  private:
    std::string _field;
    bool _ptr_access = false;
    std::unique_ptr<ASTNode> _varaible;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_ACCESS_EXPR_H
