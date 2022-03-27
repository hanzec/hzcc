//
// Created by chen_ on 2022/3/25.
//
#include <cassert>
#include "AST/expr/operator/operator.h"
#ifndef MYCC_SOURCE_AST_OPERATOR_LOGICAL_H_
#define MYCC_SOURCE_AST_OPERATOR_LOGICAL_H_
namespace Mycc::AST {
class LogicalExpr : public OperatorBase {
  public:
    LogicalExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
                std::unique_ptr<ASTNode> rhs);

  protected:
    enum LogicalType {
        kAnd,
        kOr,
        kEqual,
        kNotEqual,
    };

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    LogicalType _type;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_LOGICAL_H_
