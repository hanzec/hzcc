//
// Created by chen_ on 2022/3/25.
//
#include <string>

#include "AST/expr/operator/operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_UNARY_H_
#define MYCC_SOURCE_AST_OPERATOR_UNARY_H_
namespace Hzcc::AST {
class UnaryExpr : public ASTNode {
  public:
    enum UnaryType {
        kUnaryMinus,
        kPreInc,
        kPreDec,
        kPostInc,
        kPostDec,
        kReference,
        kDereference,
        kLogicalNot,
        kBitwiseNot,
    };

    Status visit(ASTVisitor& visitor) override;

    UnaryExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> expr);

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  protected:
    [[nodiscard]] const char* GetNodeName() const override;

  private:
    UnaryType _type;
    std::unique_ptr<ASTNode> _expr;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_UNARY_H_
