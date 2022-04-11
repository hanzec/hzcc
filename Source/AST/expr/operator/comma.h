//
// Created by chen_ on 2022/3/25.
//
#include <memory>

#include "AST/ASTNode.h"
#include "AST/expr/operator/operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_COMMA_H_
#define MYCC_SOURCE_AST_OPERATOR_COMMA_H_
namespace Mycc::AST {
class CommaExpr : public OperatorBase {
  public:
    CommaExpr(const Lexical::Token& token, std::unique_ptr<ASTNode> lhs,
              std::unique_ptr<ASTNode> rhs);

    void visit(ASTVisitor& visitor) override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_COMMA_H_
