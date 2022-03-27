//
// Created by chen_ on 2022/3/25.
//
#include <cassert>
#include <memory>

#include "AST/ast_node.h"
#include "AST/expr/operator/operator.h"
#ifndef MYCC_SOURCE_AST_OPERATOR_ASSIGN_H_
#define MYCC_SOURCE_AST_OPERATOR_ASSIGN_H_
namespace Mycc::AST {
class AssignExpr : public OperatorBase {
  public:
    AssignExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
               std::unique_ptr<ASTNode> rhs);

  protected:
    enum AssignType {
        kAssign,
        kAddAssign,
        kSubAssign,
        kMulAssign,
        kDivAssign,
        kModAssign,
        kLShiftAssign,
        kRShiftAssign,
        kAndAssign,
        kOrAssign,
        kXorAssign,
    };

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    AssignType _type;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ASSIGN_H_
