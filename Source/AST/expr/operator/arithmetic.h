//
// Created by chen_ on 2022/3/25.
//
#include <cassert>

#include "AST/expr/operator/operator.h"
#ifndef MYCC_SOURCE_AST_OPERATOR_ARITHMETIC_H_
#define MYCC_SOURCE_AST_OPERATOR_ARITHMETIC_H_
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
    ArithmeticExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
                   std::unique_ptr<ASTNode> rhs);

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] ArithmeticType GetOpType() const;

    [[nodiscard]] const char* NodeName() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  protected:
    [[nodiscard]] std::string PrintAdditionalInfo(
        const std::string& ident) const override;

  private:
    ArithmeticType _type;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ARITHMETIC_H_
