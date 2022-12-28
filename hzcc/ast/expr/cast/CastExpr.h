//
// Created by chen_ on 2022/3/25.
//
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#ifndef HZCC_AST_EXPR_CAST_EXPR_H
#define HZCC_AST_EXPR_CAST_EXPR_H
namespace hzcc::ast {
class CastExpr : public Expr {
  public:
    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Determine whether the node is a literal node or not
     * @return true if the node is a literal node, false otherwise
     */
    [[nodiscard]] bool IsLiteral() const override;

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override;

    /**
     * @brief return the expression that is being casted
     * @return the expression that is being casted
     */
    [[nodiscard]] const std::unique_ptr<Expr>& GetCastExpr() const;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  protected:
    /**
     * @brief CastExpr constructor. Will cast expression to the given
     * type. If runtime assert is enabled, will trigger an error if the given
     * type is same as the expression's type. This abstract class should never
     * initialize directly.
     * @param type the type to cast to
     * @param expr the expression to cast
     * @param location location of the cast operator
     * @param node_name the name of the cast node
     */
    CastExpr(const char* node_name,        // NOLINT
             const Position& location,     // NOLINT
             std::unique_ptr<Expr> expr);  // NOLINT
  private:
    const std::unique_ptr<Expr> _expr;
};

class ExplicitCastExpr : public CastExpr {
  public:
    /**
     * @brief CastExpr constructor. Will cast expression to the given
     * type. If runtime assert is enabled, will trigger an error if the given
     * type is same as the expression's type.
     * @param type the type to cast to
     * @param expr the expression to cast
     * @param location location of the cast operator
     */
    ExplicitCastExpr(const Position& location,     // NOLINT
                     std::shared_ptr<Type> type,   // NOLINT
                     std::unique_ptr<Expr> expr);  // NOLINT

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

  protected:
    /**
     * @brief An override function using print extra information when call
     * Stmt->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;

  private:
    const std::shared_ptr<Type> _cast_type;
};

class ImplicitCastExpr : public CastExpr {
  public:
    /**
     * @brief CastExpr constructor. Will cast expression to the given
     * to_type. If runtime assert is enabled, will trigger an error if the given
     * to_type is same as the expression's to_type.
     * @param to_type the to_type to cast to
     * @param expr the expression to cast
     * @param location location of the cast operator
     */
    ImplicitCastExpr(std::string name,             // NOLINT
                     const Position& location,     // NOLINT
                     std::unique_ptr<Expr> expr);  // NOLINT

  protected:
    /**
     * @brief An override function using print extra information when call
     * Stmt->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;

  private:
    const std::string _name;
};

class LvalueToRvalueCast : public ImplicitCastExpr {
  public:
    /**
     * @brief Construct a new LvalueToRvalue Cast. The expression has to be an
     * rvalue otherwise it will throw an runtime error.
     * @param location The location of the cast operator.
     * @param expr The expression to cast.
     */
    LvalueToRvalueCast(const Position& location,     // NOLINT
                       std::unique_ptr<Expr> expr);  // NOLINT

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override;

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

};

class IntegralCast : public ImplicitCastExpr {
  public:
    /**
     * @brief Construct a new Integral Cast.
     * @param location The location of the cast operator.
     * @param expr The expression to cast.
     * @param type The type to cast to.
     */
    IntegralCast(const Position& location,     // NOLINT
                 std::shared_ptr<Type> type,   // NOLINT
                 std::unique_ptr<Expr> expr);  // NOLINT

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

  private:
    std::shared_ptr<Type> _type;
};

class ArrayToPointerDecayCast : public ImplicitCastExpr {
  public:
    /**
     * @brief Construct a new ArrayToPointerDecay Cast. The expression has to be
     * an rvalue otherwise it will throw an runtime error.
     * @param location The location of the cast operator.
     * @param expr The expression to cast.
     */
    ArrayToPointerDecayCast(const Position& location,     // NOLINT
                            std::unique_ptr<Expr> expr);  // NOLINT

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;
};

}  // namespace hzcc::ast
#endif  // HZCC_AST_EXPR_CAST_EXPR_H
