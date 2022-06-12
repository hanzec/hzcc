//
// Created by Hanze Chen on 2022/3/25.
//
#include "AST/expr/op/OperatorBase.h"
#ifndef HZCC_AST_EXPR_OP_RELATIONAL_EXPR_H
#define HZCC_AST_EXPR_OP_RELATIONAL_EXPR_H
namespace Hzcc::AST {
enum PACKED RelationalType {
    kRelationalType_Less = 0,
    kRelationalType_Equal = 1,
    kRelationalType_NEqual = 2,
    kRelationalType_Greater = 3,
    kRelationalType_LessEqual = 4,
    kRelationalType_GreaterEqual = 5,
    kRelationalType_ENUM_SIZE = 6,  // keep last
};

class RelationalExpr : public OperatorBase {
  public:
    /**
     * @brief Construct a new Relational Expr object. Supported operators are
     * listed in following table:
     *      Operator  | Description
     *      --------- | -----------
     *      <         | Less than
     *      ==        | Equal to
     *      !=        | Not equal to
     *      >         | Greater than
     *      <=        | Less than or equal to
     *      >=        | Greater than or equal to
     * Runtime assertion will be triggered if the operator is not supported or
     * type string has a length not equal to 2.
     *
     * @param type std::string_view object for the relational type
     * @param lhs Left hand side expression
     * @param rhs Right hand side expression
     * @param loc Location of the relational expression
     */
    RelationalExpr(const Position& loc, const std::string_view& type,
                   std::unique_ptr<ASTNode> lhs,
                   std::unique_ptr<ASTNode> rhs);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Get the type of the relational expression
     * @return RelationalType of the expression
     */
    [[nodiscard]] RelationalType OpType() const;

    /**
     * @brief Get the type of the current expression. The RelationalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of char
     */
    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "RelationalExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

    /**
     * @brief An override function using print extra information when call
     * ASTNode->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     * @return std::string generated string
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;

  private:
    RelationalType _type;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_OP_RELATIONAL_EXPR_H
