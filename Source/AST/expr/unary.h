//
// Created by chen_ on 2022/3/25.
//
#include <string>

#include "AST/expr/operator/operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_UNARY_H_
#define MYCC_SOURCE_AST_OPERATOR_UNARY_H_
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
    Status visit(ASTVisitor& visitor) override;

    UnaryExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> expr);

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    [[nodiscard]] std::unique_ptr<ASTNode>& GetExpr();

    [[nodiscard]] UnaryType GetUnaryType() const;

  protected:
    [[nodiscard]] const char* NodeName() const override;

    [[nodiscard]] std::string PrintAdditionalInfo(
        const std::string& ident) const override;

  private:
    UnaryType _type;
    std::unique_ptr<ASTNode> _expr;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_UNARY_H_
