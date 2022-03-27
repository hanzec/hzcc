//
// Created by chen_ on 2022/3/25.
//
#include "AST/expr/operator/operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_BITWISE_H_
#define MYCC_SOURCE_AST_OPERATOR_BITWISE_H_
namespace Mycc::AST {
class BitwiseExpr : public OperatorBase {
  public:
    BitwiseExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
                std::unique_ptr<ASTNode> rhs);

  protected:
    enum BitwiseType {
        kAND,
        kOR,
        kXOR,
        kLSHIFT,
        kRSHIFT,
    };

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    BitwiseType _type;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_BITWISE_H_
