//
// Created by chen_ on 2022/3/25.
//
#include "AST/expr/op/OperatorBase.h"
#ifndef HZCC_AST_EXPT_UNARY_H
#define HZCC_AST_EXPT_UNARY_H
namespace Hzcc::AST {
enum PACKED UnaryType {
    kUnaryType_UnaryMinus = 0,
    kUnaryType_PreInc = 1,
    kUnaryType_PreDec = 2,
    kUnaryType_PostInc = 3,
    kUnaryType_PostDec = 4,
    kUnaryType_Reference = 5,
    kUnaryType_Dereference = 6,
    kUnaryType_LogicalNot = 7,
    kUnaryType_BitwiseNot = 8,
    kUnaryType_ENUM_SIZE = 9
};

class UnaryExpr : public ASTNode {
  public:
    UnaryExpr(const std::string_view& type,          // NOLINT
              std::unique_ptr<ASTNode> expr,         // NOLINT
              const std::pair<int, int>& location);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

    [[nodiscard]] std::unique_ptr<ASTNode>& GetExpr();

    [[nodiscard]] UnaryType GetUnaryType() const;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "UnaryExpr"
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
    UnaryType _type;
    std::unique_ptr<ASTNode> _expr;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPT_UNARY_H
