//
// Created by chen_ on 2022/3/25.
//
#include <string>

#include "AST/ASTNode.h"

#ifndef HZCC_AST_EXPR_LITERAL_EXPR_H
#define HZCC_AST_EXPR_LITERAL_EXPR_H
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
     * @param location The location of this node in source code
     */
    LiteralExpr(int64_t value, const Position& location);

    /**
     * @brief Construct a new Literal Expr object from Token
     * @param type The literal type
     * @param value The literal value
     * @param location The location of this literal
     */
    LiteralExpr(LiteralType type,                      // NOLINT
                const std::string_view& value,         // NOLINT
                const std::pair<int, int>& location);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or register value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override;

    /**
     * @brief Determine whether the node is a literal node or not
     * @return true if the node is a literal node, false otherwise
     */
    [[nodiscard]] bool IsLiteral() const override;

    [[nodiscard]] LiteralType GetLiteralType() const;

    [[nodiscard]] const std::string& GetValue() const;

    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "LiteralExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

    /**
     * @brief An override function using print extra information when call
     * ASTNode->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     * @return std::string generated string
     */
    [[nodiscard]] std::string PrintDetail(
        const std::string& ident) const override;

  private:
    std::string _value;
    enum LiteralType _type;
};

}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_LITERAL_EXPR_H
