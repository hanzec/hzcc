//
// Created by Hanze Chen on 2022/3/25.
//
#include "AST/expr/op/OperatorBase.h"
#ifndef HZCC_AST_EXPR_OP_LOGICAL_EXPR_H
#define HZCC_AST_EXPR_OP_LOGICAL_EXPR_H
namespace Hzcc::AST {
enum PACKED LogicalType {
    kLogicalType_And = 0,
    kLogicalType_Or = 1,
    kLogicalType_ENUM_SIZE = 2,
};

class LogicalExpr : public OperatorBase {
  public:
    LogicalExpr(const std::string_view& type,  // NOLINT
                std::unique_ptr<ASTNode> lhs,  // NOLINT
                std::unique_ptr<ASTNode> rhs,  // NOLINT
                const Position& loc);          // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Get type of the logical expression
     * @return type of the logical expression
     */
    [[nodiscard]] LogicalType GetLogicalType() const;

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of char
     */
    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "LogicalExpr"
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
    LogicalType _type;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_OP_LOGICAL_EXPR_H
