//
// Created by Hanze Chen on 2022/3/25.
//
#include "AST/expr/op/OperatorBase.h"
#ifndef HZCC_AST_EXPR_OP_ASSIGN_EXPR_H
#define HZCC_AST_EXPR_OP_ASSIGN_EXPR_H
namespace Hzcc::AST {
enum PACKED AssignType {
    kAssignType_Assign = 0,
    kAssignType_AddAssign = 1,
    kAssignType_SubAssign = 2,
    kAssignType_MulAssign = 3,
    kAssignType_DivAssign = 4,
    kAssignType_ModAssign = 5,
    kAssignType_LShiftAssign = 6,
    kAssignType_RShiftAssign = 7,
    kAssignType_AndAssign = 8,
    kAssignType_OrAssign = 9,
    kAssignType_XorAssign = 10,
    kAssignType_Max = 11  // keep last
};

class AssignExpr : public OperatorBase {
  public:
    /**
     * @brief Constructor for Assign Expression AST Node, supported operators
     * are listed in following table:
     *    Operator  | Description
     *    --------- | -----------
     *    =         | Assignment
     *    +=        | Addition Assignment
     *    -=        | Subtraction Assignment
     *    *=        | Multiplication Assignment
     *    /=        | Division Assignment
     *    %=        | Modulus Assignment
     *    <<=       | Left Shift Assignment
     *    >>=       | Right Shift Assignment
     *    &=        | Bitwise AND Assignment
     *    |=        | Bitwise OR Assignment
     *    ^=        | Bitwise XOR Assignment
     * Runtime assertion will be triggered if the operator is not supported. Or
     * type string has a length > 3
     * @param type Lexical::Token object for the assign type
     * @param lhs  Left hand side expression
     * @param rhs  Right hand side expression
     * @param location Location of the assign expression
     */
    AssignExpr(const Position& location,       // NOLINT
               const std::string_view& type,   // NOLINT
               std::unique_ptr<ASTNode> lhs,   // NOLINT
               std::unique_ptr<ASTNode> rhs);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Get the assign type
     * @return AssignType enum value
     */
    [[nodiscard]] AssignType GetAssignType() const;

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

    /**
     * @brief Get Name of the node
     * @return will always return "AssignExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

  private:
    AssignType _type;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_OP_ASSIGN_EXPR_H
