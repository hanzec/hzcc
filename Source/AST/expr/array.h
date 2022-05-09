//
// Created by chen_ on 2022/3/25.
//
#include <string>
#include <utility>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_ARRAY_H_
#define MYCC_SOURCE_AST_OPERATOR_ARRAY_H_
namespace Hzcc::AST {
class ArraySubscriptExpr : public ASTNode {
  public:
    ArraySubscriptExpr(const Lexical::Token& token,
                       std::unique_ptr<AST::ASTNode> name,
                       std::unique_ptr<AST::ASTNode> index);

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool IsAssignable() const override;

    [[nodiscard]] bool IsDereference() const override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    [[nodiscard]] std::unique_ptr<AST::ASTNode>& GetArrayBase();

    [[nodiscard]] std::unique_ptr<AST::ASTNode>& GetSubscript();

    [[nodiscard]] const char* NodeName() const override;

  protected:
    [[nodiscard]] std::string PrintAdditionalInfo(
        const std::string& ident) const override;

  private:
    std::unique_ptr<AST::ASTNode> _name;
    std::unique_ptr<AST::ASTNode> _index_expr;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ARRAY_H_
