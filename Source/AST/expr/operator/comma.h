//
// Created by chen_ on 2022/3/25.
//
#include <memory>

#include "AST/ASTNode.h"
#include "AST/expr/operator/operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_COMMA_H_
#define MYCC_SOURCE_AST_OPERATOR_COMMA_H_
namespace Hzcc::AST {
class CommaExpr : public OperatorBase {
  public:
    CommaExpr(const Lexical::Token& token, std::unique_ptr<ASTNode> lhs,
              std::unique_ptr<ASTNode> rhs);

    Status visit(ASTVisitor& visitor) override;

  protected:
    [[nodiscard]] const char* NodeName() const override;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_COMMA_H_
