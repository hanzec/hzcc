//
// Created by chen_ on 2022/3/25.
//
#include <string>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_VALUE_VALUE_H_
#define MYCC_SOURCE_AST_VALUE_VALUE_H_
namespace Hzcc::AST {
class LiteralExpr : public ASTNode {
  public:
    enum ValueType { kChar, kInteger, kReal_number, kString };

    explicit LiteralExpr(int64_t);

    LiteralExpr(ValueType type, const Lexical::Token& value);

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool IsAssignable() const override;

    [[nodiscard]] bool IsLiteral() const override { return true; }

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;
    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    ValueType _type;
    std::string _value;
};

}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_VALUE_VALUE_H_
