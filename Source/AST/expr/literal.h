//
// Created by chen_ on 2022/3/25.
//
#include <string>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_VALUE_VALUE_H_
#define MYCC_SOURCE_AST_VALUE_VALUE_H_
namespace Hzcc::AST {
enum PACKED LiteralType {
    kLiteralType_Char = 0,
    kLiteralType_Integer = 1,
    kLiteralType_Real_number = 2,
    kLiteralType_String = 3,
    kLiteralType_Max = 4  // keep last
};

class LiteralExpr : public ASTNode {
  public:
    /**
     * @brief Construct a new Literal Expr object from integer value
     * @note The LiteralTpe from this constructor is kLiteralType_Integer
     * @param value The integer value
     */
    explicit LiteralExpr(int64_t);

    /**
     * @brief Construct a new Literal Expr object from Token
     * @param token The Token contains the literal value
     * @param type The literal type
     */
    LiteralExpr(LiteralType type, const Lexical::Token& value);

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool IsAssignable() const override;

    [[nodiscard]] bool IsLiteral() const override;

    [[nodiscard]] LiteralType LiteralType() const;

    [[nodiscard]] const std::string& GetValue() const;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  protected:
    [[nodiscard]] const char* GetNodeName() const override;
    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    std::string _value;
    enum LiteralType _type;
};

}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_VALUE_VALUE_H_
