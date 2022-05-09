//
// Created by chen_ on 2022/3/25.
//
#include "AST/expr/operator/operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_BITWISE_H_
#define MYCC_SOURCE_AST_OPERATOR_BITWISE_H_
namespace Hzcc::AST {
class BitwiseExpr : public OperatorBase {
  public:
    BitwiseExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
                std::unique_ptr<ASTNode> rhs);

    Status visit(ASTVisitor& visitor) override;

  protected:
    enum BitwiseType {
        kAND,
        kOR,
        kXOR,
        kLSHIFT,
        kRSHIFT,
    };

  protected:
    [[nodiscard]] const char* NodeName() const override;

  private:
    BitwiseType _type;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_BITWISE_H_
