//
// Created by chen_ on 2022/3/25.
//
#include "AST/ASTNode.h"

#ifndef HZCC_AST_EXPR_ARRAY_SUBSCRIPT_EXPR_H
#define HZCC_AST_EXPR_ARRAY_SUBSCRIPT_EXPR_H
namespace Hzcc::AST {
class ArraySubscriptExpr : public ASTNode {
  public:
    /**
     * @brief ArraySubscriptExpr constructor. Create a new ArraySubscriptExpr
     * node. At runtime will check whether the array have a type of array or
     * not.
     * @param location location of the array subscript expression
     * @param array the array array expression
     * @param subscript the array subscript expression
     */
    ArraySubscriptExpr(const Position& location, std::unique_ptr<ASTNode> array,
                       std::unique_ptr<ASTNode> subscript);  // NOLINT

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

    [[nodiscard]] bool IsDereference() const override;

    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

    [[nodiscard]] std::unique_ptr<AST::ASTNode>& GetArrayBase();

    [[nodiscard]] std::unique_ptr<AST::ASTNode>& GetSubscript();

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "ArraySubscriptExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

    /**
     * @brief An override function using print extra information when call
     * ASTNode->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;

  private:
    std::unique_ptr<AST::ASTNode> _array;
    std::unique_ptr<AST::ASTNode> _subscript;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_ARRAY_SUBSCRIPT_EXPR_H
