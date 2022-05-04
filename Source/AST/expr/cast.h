//
// Created by chen_ on 2022/3/25.
//
#include <memory>
#include <utility>

#include "AST/ASTNode.h"
#include "AST/type/Type.h"
#ifndef MYCC_SOURCE_AST_OPERATOR_CAST_H_
#define MYCC_SOURCE_AST_OPERATOR_CAST_H_
namespace Hzcc::AST {
class CastExpr : public ASTNode {
  public:
    CastExpr(std::shared_ptr<Type> type, std::unique_ptr<ASTNode> expr,
             std::pair<int, int> location)
        : ASTNode(location),
          _cast_type(std::move(type)),
          _cast_expr(std::move(expr)) {}

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool IsAssignable() const override;

    [[nodiscard]] bool IsLiteral() const override {
        return _cast_expr->IsLiteral();
    }

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    std::shared_ptr<Type> _cast_type;
    std::unique_ptr<ASTNode> _cast_expr;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_CAST_H_
