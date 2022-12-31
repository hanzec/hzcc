//
// Created by hanzech on 12/19/22.
//
#include "ast/Stmt.h"

#ifndef HZCC_EXPR_H
#define HZCC_EXPR_H
namespace hzcc::ast {
using ExprPtr = std::unique_ptr<Expr>;

class OperatorBase : public Expr {
  public:
    /**
     * @brief Get the type of the current expression. The type of OperatorBase
     * and its inherited class will determine by LHS's and RHS's deduced type:
     *  1. If both LHS and RHS have the deduced type, then the final type will
     *  be same as LHS
     *  2. If only LHS has the deduced type or only RHS has the deduced type,
     *  then the returned type will be same as the expression that can not be
     *  deduced.
     * @return return the type of the current expression
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    /**
     * @brief Get left hand side expression
     * @return left hand side expression
     */
    [[nodiscard]] std::unique_ptr<Expr>& lhs();

    /**
     * @brief Get the right hand side expression
     * @return right hand side expression
     */
    [[nodiscard]] std::unique_ptr<Expr>& rhs();

  protected:
    /**
     * @brief OperatorBase constructor, this abstract class should never
     * initialized directly.
     * @param lhs left hand side expression
     * @param rhs right hand side expression
     * @param loc location of operator
     */
    OperatorBase(const Position& loc,         // NOLINT
                 const char* node_name,       // NOLINT
                 std::unique_ptr<Expr> lhs,   // NOLINT
                 std::unique_ptr<Expr> rhs);  // NOLINT

    [[nodiscard]] const std::unique_ptr<Expr>& lhs_c() const;
    [[nodiscard]] const std::unique_ptr<Expr>& rhs_c() const;

  private:
    std::unique_ptr<Expr> _lhs;
    std::unique_ptr<Expr> _rhs;
};

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
    [[nodiscard]] const std::unique_ptr<Expr>& cast_expr() const;

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

    [[nodiscard]] std::unique_ptr<Expr>& expr();

    [[nodiscard]] UnaryType GetUnaryType() const;

    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

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

    [[nodiscard]] std::unique_ptr<Expr>& cond_expr();

    [[nodiscard]] std::unique_ptr<Expr>& true_expr();

    [[nodiscard]] std::unique_ptr<Expr>& false_expr();

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

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
};

class SizeofExpr : public Expr {
  public:
    /**
     * @brief Construct a new Sizeof Expr object
     * @param expr the expression to get the size of
     * @param location location of the sizeof operator
     */
    SizeofExpr(const Position& location,     // NOLINT
               std::unique_ptr<Stmt> expr);  // NOLINT

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

    [[nodiscard]] LiteralType literal_type() const;

    [[nodiscard]] bool IsReturnLValue() const override;

    [[nodiscard]] std::string_view get_literal_val() const;

    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

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
                 std::list<std::unique_ptr<Expr>> args);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] std::string_view name() const;

    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::list<std::unique_ptr<Expr>>& args();

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  private:
    const std::string _name;
    std::shared_ptr<Type> _ret_type;
    std::list<std::unique_ptr<Expr>> _func_args;
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
    DeclRefExpr(const Position& loc,     // NO_LINT
                std::string_view name);  // NO_LINT

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
    [[nodiscard]] std::string_view var_name() const;

    /**
     * @brief get the type of the variable
     * @return the type of the variable
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  private:
    const std::string _name;
    const std::unique_ptr<TypeProxyExpr> _type;
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

    [[nodiscard]] std::unique_ptr<Expr>& base_expr();

    [[nodiscard]] std::unique_ptr<Expr>& subscript_expr();

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  private:
    std::unique_ptr<Expr> _array;
    std::unique_ptr<Expr> _subscript;
};

class MemberExpr : public Expr {
  public:
    /**
     * @brief Construct a new AccessExpr object. The AccessExpr is used to
     * access the struct member. Runtime will check the type is struct or not if
     * enabled. Also the field F must be not empty.
     * @param isPtr whether the access expression is a pointer access
     * @param field the field name
     * @param expr if have chain access or EmptyStmt otherwise
     * @param loc location of access expression
     */
    MemberExpr(bool isPtr,                   // NO_LINT
               const Position& loc,          // NO_LINT
               std::string_view field,       // NO_LINT
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

    [[nodiscard]] std::string_view field_name() const;

    [[nodiscard]] std::unique_ptr<ast::Expr> base_expr() const;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  private:
    std::string _field;
    bool _ptr_access = false;
    std::unique_ptr<Expr> _varaible;
};

enum PACKED RelationalType {
    kRelationalType_Less = 0,
    kRelationalType_Equal = 1,
    kRelationalType_NEqual = 2,
    kRelationalType_Greater = 3,
    kRelationalType_LessEqual = 4,
    kRelationalType_GreaterEqual = 5,
    kRelationalType_ENUM_SIZE = 6,  // keep last
};

class RelationalExpr : public OperatorBase {
  public:
    /**
     * @brief Construct a new Relational Expr object. Supported operators are
     * listed in following table:
     *      Operator  | Description
     *      --------- | -----------
     *      <         | Less than
     *      ==        | Equal to
     *      !=        | Not equal to
     *      >         | Greater than
     *      <=        | Less than or equal to
     *      >=        | Greater than or equal to
     * Runtime assertion will be triggered if the operator is not supported or
     * type string has a length not equal to 2.
     *
     * @param type std::string_view object for the relational type
     * @param lhs Left hand side expression
     * @param rhs Right hand side expression
     * @param loc loc of the relational expression
     */
    RelationalExpr(const Position& loc,         // NOLINT
                   std::string_view type,       // NOLINT
                   std::unique_ptr<Expr> lhs,   // NOLINT
                   std::unique_ptr<Expr> rhs);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Get the type of the relational expression
     * @return RelationalType of the expression
     */
    [[nodiscard]] RelationalType op_type() const;

    /**
     * @brief Get the type of the current expression. The RelationalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of char
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  private:
    RelationalType _type;
};

enum PACKED LogicalType {
    kLogicalType_And = 0,
    kLogicalType_Or = 1,
    kLogicalType_ENUM_SIZE = 2,
};

class LogicalExpr : public OperatorBase {
  public:
    LogicalExpr(const Position& loc,         // NOLINT
                std::string_view type,       // NOLINT
                std::unique_ptr<Expr> lhs,   // NOLINT
                std::unique_ptr<Expr> rhs);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Get type of the logical expression
     * @return type of the logical expression
     */
    [[nodiscard]] LogicalType op_type() const;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of char
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

  private:
    LogicalType _type;
};

class CommaExpr : public OperatorBase {
  public:
    /**
     * @brief CommaExpr constructor
     * @param lhs left hand side expression
     * @param rhs right hand side expression
     * @param loc location of comma operator
     */
    CommaExpr(const Position& loc,         // NOLINT
              std::unique_ptr<Expr> lhs,   // NOLINT
              std::unique_ptr<Expr> rhs);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of char
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;
};

enum PACKED BitwiseType {
    kBitwiseType_AND = 0,
    kBitwiseType_OR = 1,
    kBitwiseType_XOR = 2,
    kBitwiseType_LSHIFT = 3,
    kBitwiseType_RSHIFT = 4,
    kBitwiseType_ENUM_SIZE = 5  // Keep last
};

class BitwiseExpr : public OperatorBase {
  public:
    /**
     * @brief Construct a new Bitwise Expr object. Supported operators are
     * listed in following table: Operator  | Description
     *   --------- | -----------
     *   &         | Bitwise AND
     *   |         | Bitwise OR
     *   ^         | Bitwise XOR
     *   <<        | Left Shift
     *   >>        | Right Shift
     *   Runtime assertion will be triggered if the operator is not supported.
     * Or type string has a length > 3
     *
     * @param loc loc of the bitwise expression
     * @param type lexical::Token object for the bitwise type
     * @param left_expr Left hand side expression
     * @param right_expr Right hand side expression
     */
    BitwiseExpr(const Position& loc,                // NOLINT
                std::string_view type,              // NOLINT
                std::unique_ptr<Expr> left_expr,    // NOLINT
                std::unique_ptr<Expr> right_expr);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of char
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

  private:
    BitwiseType _type;
};

enum PACKED AssignType {
    kAssignType_Assign = 0,
    kAssignType_AddAssign = 1,
    kAssignType_SubAssign = 2,
    kAssignType_MulAssign = 3,
    kAssignType_DivAssign = 4,
    kAssignType_ModAssign = 5,
    kAssignType_LShiftAssign = 6,
    kAssignType_RShiftAssign = 7,
    kAssignType_AndAssign = 8,
    kAssignType_OrAssign = 9,
    kAssignType_XorAssign = 10,
    kAssignType_Max = 11  // keep last
};

class AssignExpr : public OperatorBase {
  public:
    /**
     * @brief Constructor for Assign Expression ast Node, supported operators
     * are listed in following table:
     *    Operator  | Description
     *    --------- | -----------
     *    =         | Assignment
     *    +=        | Addition Assignment
     *    -=        | Subtraction Assignment
     *    *=        | Multiplication Assignment
     *    /=        | Division Assignment
     *    %=        | Modulus Assignment
     *    <<=       | Left Shift Assignment
     *    >>=       | Right Shift Assignment
     *    &=        | Bitwise AND Assignment
     *    |=        | Bitwise OR Assignment
     *    ^=        | Bitwise XOR Assignment
     * Runtime assertion will be triggered if the operator is not supported. Or
     * type string has a length > 3
     * @param type lexical::Token object for the assign type
     * @param lhs  Left hand side expression
     * @param rhs  Right hand side expression
     * @param loc loc of the assign expression
     */
    AssignExpr(const Position& loc,         // NOLINT
               std::string_view type,       // NOLINT
               std::unique_ptr<Expr> lhs,   // NOLINT
               std::unique_ptr<Expr> rhs);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Get the assign type
     * @return AssignType enum value
     */
    [[nodiscard]] AssignType op_type() const;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of char
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

  private:
    AssignType _type;
};

class ArithmeticExpr : public OperatorBase {
  public:
    /**
     * @brief Constructor of ArithmeticExpr Node. The arithmetic expression
     * support following operator symbols as input:
     *        symbol   |   Operator   |   Description
     *      ---------- | ------------ | ---------------
     *       +         |  Add         | Addition
     *       -         |  Sub         | Subtraction
     *       *         |  Mul         | Multiplication
     *       /         |  Div         | Division
     *       %         |  Mod         | Modulo
     * Runtime assertion will be triggered if the input symbol is not one of the
     * above. Or type is empty.
     * @param type arithmetic type in string format.
     * @param lhs left hand side of the expression.
     * @param rhs right hand side of the expression.
     * @param location location of the expression.
     */
    ArithmeticExpr(const Position& location, std::string_view type,
                   std::unique_ptr<Expr> lhs,
                   std::unique_ptr<Expr> rhs);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Get the type of the arithmetic expression.
     * @return Returned type of the arithmetic expression.
     */
    [[nodiscard]] ArithmeticType op_type() const;

    /**
     * @brief Get the deduced value of the arithmetic expression if both LHS and
     * RHS are deductible. Or std::nullopt otherwise.
     * @return ast::DeduceValue if deduced, std::nullopt if not.
     */
    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  private:
    ArithmeticType _type;
};

class ExplicitCastExpr : public CastExpr {
  public:
    /**
     * @brief CastExpr constructor. Will cast expression to the given
     * type. If runtime assert is enabled, will trigger an error if the given
     * type is same as the expression's type.
     * @param type the type to cast to
     * @param expr the expression to cast
     * @param loc location of the cast operator
     */
    ExplicitCastExpr(const Position& loc,                   // NOLINT
                     std::unique_ptr<Expr> expr,            // NOLINT
                     std::unique_ptr<TypeProxyExpr> type);  // NOLINT

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

  private:
    const std::unique_ptr<TypeProxyExpr> _cast_type;
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
#endif  // HZCC_EXPR_H
