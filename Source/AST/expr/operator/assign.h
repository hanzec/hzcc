//
// Created by chen_ on 2022/3/25.
//
#include <cassert>
#include <memory>

#include "AST/ASTNode.h"
#include "AST/expr/operator/operator.h"
#ifndef MYCC_SOURCE_AST_OPERATOR_ASSIGN_H_
#define MYCC_SOURCE_AST_OPERATOR_ASSIGN_H_
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
     * @brief Constructor for Assign Expression
     * Supported Assign Types:  =, +=, -=, *=, /=, %=, <<=, >>=, &=, |=, ^=
     *
     * @param type Lexical::Token object for the assign type
     * @param lhs  Left hand side expression
     * @param rhs  Right hand side expression
     */
    AssignExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
               std::unique_ptr<ASTNode> rhs);

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

    /**
     * @brief Get Name of the node
     * @return will always return "AssignExpr"
     */
    [[nodiscard]] const char* GetNodeName() const override;

  protected:
    /**
     * @brief An override function using print extra information when call
     * ASTNode->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     * @return
     */
    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    AssignType _type;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ASSIGN_H_
