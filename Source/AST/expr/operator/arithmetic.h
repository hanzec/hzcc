//
// Created by chen_ on 2022/3/25.
//
#include <cassert>

#include "AST/expr/operator/operator.h"
#ifndef MYCC_SOURCE_AST_OPERATOR_ARITHMETIC_H_
#define MYCC_SOURCE_AST_OPERATOR_ARITHMETIC_H_
namespace Mycc::AST {
class ArithmeticExpr : public OperatorBase {
  public:
    ArithmeticExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
                   std::unique_ptr<ASTNode> rhs);

    void visit(ASTVisitor& visitor) override;

  protected:
    enum ArithmeticType {
        kAdd,
        kSub,
        kMul,
        kDiv,
        kMod,
    };

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  private:
    ArithmeticType _type;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ARITHMETIC_H_
