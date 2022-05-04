//
// Created by chen_ on 2022/3/25.
//
#include <cassert>

#include "AST/expr/operator/operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_RELATIONAL_H_
#define MYCC_SOURCE_AST_OPERATOR_RELATIONAL_H_
namespace Hzcc::AST {
class RelationalExpr : public OperatorBase {
  public:
    RelationalExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
                   std::unique_ptr<ASTNode> rhs);

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  protected:
    enum RelationalType { LT, GT, LE, GE, EQ, NE };

    [[nodiscard]] std::string GetNodeName() const override;

    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    RelationalType _type;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_RELATIONAL_H_
