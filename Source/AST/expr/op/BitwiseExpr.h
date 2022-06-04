//
// Created by Hanze Chen on 2022/3/25.
//
#include "AST/expr/op/OperatorBase.h"
#ifndef HZCC_AST_EXPR_OP_BITWISE_EXPR_H
#define HZCC_AST_EXPR_OP_BITWISE_EXPR_H
namespace Hzcc::AST {
enum PACKED BitwiseType {
    kBitwiseType_AND = 0,
    kBitwiseType_OR = 1,
    kBitwiseType_XOR = 2,
    kBitwiseType_LSHIFT = 3,
    kBitwiseType_RSHIFT = 4,
    kBitwiseType_ENUM_SIZE = 5  // Keep last
};

class BitwiseExpr : public OperatorBase {
  public:
    /**
     * @brief Construct a new Bitwise Expr object. Supported operators are
     * listed in following table: Operator  | Description
     *   --------- | -----------
     *   &         | Bitwise AND
     *   |         | Bitwise OR
     *   ^         | Bitwise XOR
     *   <<        | Left Shift
     *   >>        | Right Shift
     *   Runtime assertion will be triggered if the operator is not supported.
     * Or type string has a length > 3
     *
     * @param loc Location of the bitwise expression
     * @param type Lexical::Token object for the bitwise type
     * @param lhs Left hand side expression
     * @param rhs Right hand side expression
     */
    BitwiseExpr(const Position& loc, const std::string_view& type,
                std::unique_ptr<ASTNode> lhs,
                std::unique_ptr<ASTNode> rhs);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "BitwiseExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

  private:
    BitwiseType _type;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_OP_BITWISE_EXPR_H
