//
// Created by chen_ on 2022/3/25.
//
#include <string>
#include <utility>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_ARRAY_H_
#define MYCC_SOURCE_AST_OPERATOR_ARRAY_H_
namespace Mycc::AST {
class ArraySubscriptExpr : public ASTNode {
  public:
    ArraySubscriptExpr(const Lexical::Token& token,
                       std::unique_ptr<AST::ASTNode> name,
                       std::unique_ptr<AST::ASTNode> index);

    void visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool IsDeducible() const override { return true; }

    [[nodiscard]] bool IsAssignable() const override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;
    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    std::unique_ptr<AST::ASTNode> _name;
    std::unique_ptr<AST::ASTNode> _index_expr;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ARRAY_H_
