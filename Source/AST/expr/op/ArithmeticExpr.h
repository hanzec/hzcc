//
// Created by Hanze Chen on 2022/3/25.
//
#include "AST/expr/op/OperatorBase.h"
#ifndef HZCC_AST_EXPR_OP_ARITHMETIC_TYPE_H
#define HZCC_AST_EXPR_OP_ARITHMETIC_TYPE_H
namespace Hzcc::AST {

enum PACKED ArithmeticType {
    kArithmeticType_Add = 0,
    kArithmeticType_Sub = 1,
    kArithmeticType_Mul = 2,
    kArithmeticType_Div = 3,
    kArithmeticType_Mod = 4,
    kArithmeticType_ENUM_SIZE = 5  // Should always be the last one
};

class ArithmeticExpr : public OperatorBase {
  public:
    /**
     * @brief Constructor of ArithmeticExpr Node. The arithmetic expression
     * support following operator symbols as input:
     *        symbol   |   Operator   |   Description
     *      ---------- | ------------ | ---------------
     *       +         |  Add         | Addition
     *       -         |  Sub         | Subtraction
     *       *         |  Mul         | Multiplication
     *       /         |  Div         | Division
     *       %         |  Mod         | Modulo
     * Runtime assertion will be triggered if the input symbol is not one of the
     * above. Or type is empty.
     * @param type arithmetic type in string format.
     * @param lhs left hand side of the expression.
     * @param rhs right hand side of the expression.
     * @param location location of the expression.
     */
    ArithmeticExpr(const Position& location, const std::string_view& type,
                   std::unique_ptr<ASTNode> lhs,
                   std::unique_ptr<ASTNode> rhs);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Get the type of the arithmetic expression.
     * @return Returned type of the arithmetic expression.
     */
    [[nodiscard]] ArithmeticType GetOpType() const;

    /**
     * @brief Get the deduced value of the arithmetic expression if both LHS and
     * RHS are deductible. Or std::nullopt otherwise.
     * @return AST::DeduceValue if deduced, std::nullopt if not.
     */
    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "ArithmeticExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

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
    ArithmeticType _type;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_OP_ARITHMETIC_TYPE_H
