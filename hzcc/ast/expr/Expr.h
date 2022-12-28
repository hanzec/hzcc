//
// Created by hanzech on 12/19/22.
//
#include "ast/Stmt.h"

#ifndef HZCC_EXPR_H
#define HZCC_EXPR_H
namespace hzcc::ast {

using ExprPtr = std::unique_ptr<Expr>;

class Expr : public Stmt {
  public:
    /**
     * @brief determine whether the node will return a location value (LValue)
     * or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */

    [[nodiscard]] virtual bool IsReturnLValue() const { return false; }

    [[nodiscard]] virtual std::optional<DeduceValue> GetDeducedValue()
        const = 0;

    /**
     * @brief Determine whether the node is used to access element in array or
     * not.
     * @return
     */

    [[nodiscard]] virtual bool IsDereference() const { return false; }

    /**
     * @brief Get the return type of current node
     * @return the type of the node if the node have a return type, otherwise
     * return an void type. Return nullptr if has internal_status error.
     */
    [[nodiscard]] virtual std::shared_ptr<Type> retType() const = 0;

  protected:
    Expr(const Position& loc, const char* name) : Stmt(loc, name) {}
};

enum PACKED UnaryType {
    kUnaryType_UnaryMinus = 0,
    kUnaryType_PreInc = 1,
    kUnaryType_PreDec = 2,
    kUnaryType_PostInc = 3,
    kUnaryType_PostDec = 4,
    kUnaryType_Reference = 5,
    kUnaryType_Dereference = 6,
    kUnaryType_LogicalNot = 7,
    kUnaryType_BitwiseNot = 8,
    kUnaryType_ENUM_SIZE = 9
};

class UnaryOperator : public Expr {
  public:
    UnaryOperator(const Position& loc,          // NOLINT
                  std::string_view type,        // NOLINT
                  std::unique_ptr<Expr> expr);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] std::unique_ptr<Expr>& GetExpr();

    [[nodiscard]] UnaryType GetUnaryType() const;

    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

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
    UnaryType _type;
    std::unique_ptr<Expr> _expr;
};

class TernaryExpr : public Expr {
  public:
    TernaryExpr(const Position& location,           // NOLINT
                std::unique_ptr<Expr> cond,         // NOLINT
                std::unique_ptr<Expr> true_expr,    // NOLINT
                std::unique_ptr<Expr> false_expr);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::unique_ptr<Expr>& GetContStmt();

    [[nodiscard]] std::unique_ptr<Expr>& GetTrueExpr();

    [[nodiscard]] std::unique_ptr<Expr>& GetFalseExpr();

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  protected:
    /**
     * @brief An override function using print extra information when call
     * Stmt->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     * @return std::string generated string
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;

  private:
    std::unique_ptr<Expr> _cond;
    std::unique_ptr<Expr> _true_expr;
    std::unique_ptr<Expr> _false_expr;
};

class EmptyExpr : public Expr {
  public:
    EmptyExpr();

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Return type of the SizeofExpr
     * @return will always return "int"
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  protected:
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;
};

class SizeofExpr : public Expr {
  public:
    /**
     * @brief Construct a new Sizeof Expr object
     * @param expr the expression to get the size of
     * @param location location of the sizeof operator
     */
    SizeofExpr(std::unique_ptr<Stmt> expr, const Position& location);

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Return type of the SizeofExpr
     * @return will always return "int"
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

  private:
    std::unique_ptr<Stmt> _expr;
};

enum PACKED LiteralType {
    kLiteralType_Char = 0,
    kLiteralType_Integer = 1,
    kLiteralType_Real_number = 2,
    kLiteralType_String = 3,
    kLiteralType_Max = 4  // keep last
};

class LiteralExpr : public Expr {
  public:
    /**
     * @brief Construct a new Literal Expr object from integer value
     * @note The LiteralTpe from this constructor is kLiteralType_Integer
     * @param value The integer value
     * @param location The location of this node in source code
     */
    LiteralExpr(int64_t value,              // NOLINT
                const Position& location);  // NOLINT

    /**
     * @brief Construct a new Literal Expr object from Token
     * @param type The literal type
     * @param value The literal value
     * @param loc The location of this literal
     */
    LiteralExpr(LiteralType type,         // NOLINT
                const Position& loc,      // NOLINT
                std::string_view value);  // NOLINT

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

    [[nodiscard]] LiteralType GetLiteralType() const;

    [[nodiscard]] bool IsReturnLValue() const override;

    [[nodiscard]] const std::string& GetValue() const;

    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  protected:
    /**
     * @brief An override function using print extra information when call
     * Stmt->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     * @return std::string generated string
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;

  private:
    std::string _value;
    enum LiteralType _type;
};

class FuncCallStmt : public Expr {
  public:
    /**
     * @brief Constructor of FuncCallStmt
     * @param type type of the function call
     * @param name name of the function call
     * @param loc location of the function call
     * @param args arguments of the function call
     */
    FuncCallStmt(const Position& loc,                     // NOLINT
                 std::string_view name,                   // NOLINT
                 std::shared_ptr<Type> type,              // NOLINT
                 std::list<std::unique_ptr<Expr>> args);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] const std::string& FuncName() const;

    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] const std::list<std::unique_ptr<Expr>>& GetArgsNode();

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

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
    const std::shared_ptr<Type> _return_type;
    const std::list<std::unique_ptr<Expr>> _args;
};

class DeclRefExpr : public Expr {
  public:
    /**
     * @brief Constructor of DeclRefExpr. If runtime assert is enabled, will
     * trigger an error if the given name is empty string.
     * @param loc location of the node
     * @param type type of the variable
     * @param name The name of the variable.
     */
    DeclRefExpr(const Position& loc,          // NO_LINT
                std::string_view name,        // NO_LINT
                std::shared_ptr<Type> type);  // NO_LINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override;

    /**
     * @brief get the name of the variable
     * @return the name of the variable
     */
    [[nodiscard]] const std::string& VarName() const;

    /**
     * @brief get the type of the variable
     * @return the type of the variable
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  protected:
    /**
     * @brief An override function using print extra information when call
     * Stmt->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     * @return std::string generated string
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;

  private:
    const std::string _name;
    const std::shared_ptr<Type> _type;
};

class ArraySubscriptExpr : public Expr {
  public:
    /**
     * @brief ArraySubscriptExpr constructor. Create a new ArraySubscriptExpr
     * node. At runtime will check whether the array have a type of array or
     * not.
     * @param location location of the array subscript expression
     * @param array the array array expression
     * @param subscript the array subscript expression
     */
    ArraySubscriptExpr(const Position& location,          // NOLINT
                       std::unique_ptr<Expr> array,       // NOLINT
                       std::unique_ptr<Expr> subscript);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override;

    [[nodiscard]] bool IsDereference() const override;

    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::unique_ptr<Expr>& GetArrayBase();

    [[nodiscard]] std::unique_ptr<Expr>& GetSubscript();

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

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
    std::unique_ptr<Expr> _array;
    std::unique_ptr<Expr> _subscript;
};

class AccessExpr : public Expr {
  public:
    /**
     * @brief Construct a new AccessExpr object. The AccessExpr is used to
     * access the struct member. Runtime will check the type is struct or not if
     * enabled. Also the field name must be not empty.
     * @param isPtr whether the access expression is a pointer access
     * @param field the field name
     * @param expr if have chain access or EmptyStmt otherwise
     * @param loc location of access expression
     */
    AccessExpr(bool isPtr, const Position& loc, std::string_view field,
               std::unique_ptr<Expr> expr);  // NO_LINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override;

    /**
     * @brief Get the type of specified field.
     * @return he type of specified field.
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

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
    std::string _field;
    bool _ptr_access = false;
    std::unique_ptr<Expr> _varaible;
};

}  // namespace hzcc::ast
#endif  // HZCC_EXPR_H
