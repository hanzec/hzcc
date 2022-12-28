//
// Created by Hanze Chen on 2022/3/25.
//
#include "ast/expr/Expr.h"
#ifndef HZCC_AST_EXPR_OP_OPERATOR_BASE_H
#define HZCC_AST_EXPR_OP_OPERATOR_BASE_H
namespace hzcc::ast {
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
    [[nodiscard]] const std::unique_ptr<Expr>& GetLHS() const;

    /**
     * @brief Get the right hand side expression
     * @return right hand side expression
     */
    [[nodiscard]] const std::unique_ptr<Expr>& GetRHS() const;

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

  private:
    std::unique_ptr<Expr> _lhs;
    std::unique_ptr<Expr> _rhs;
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
     * @param loc Location of the relational expression
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
    [[nodiscard]] RelationalType OpType() const;

    /**
     * @brief Get the type of the current expression. The RelationalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of char
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

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
    RelationalType _type;
};

enum PACKED LogicalType {
    kLogicalType_And = 0,
    kLogicalType_Or = 1,
    kLogicalType_ENUM_SIZE = 2,
};

class LogicalExpr : public OperatorBase {
  public:
    LogicalExpr(const Position& loc, std::string_view type,
                std::unique_ptr<Expr> lhs,
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
    [[nodiscard]] LogicalType GetLogicalType() const;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of char
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

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
     * @param loc Location of the bitwise expression
     * @param type lexical::Token object for the bitwise type
     * @param left_expr Left hand side expression
     * @param right_expr Right hand side expression
     */
    BitwiseExpr(const char* type,                   // NOLINT
                const Position& loc,                // NOLINT
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
     * @param loc Location of the assign expression
     */
    AssignExpr(const Position& loc, std::string_view type,
               std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs);  // NOLINT

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
    [[nodiscard]] AssignType GetAssignType() const;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of char
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

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
    AssignType _type;
};

enum PACKED ArithmeticType {
    kArithmeticType_Add = 0,
    kArithmeticType_Sub = 1,
    kArithmeticType_Mul = 2,
    kArithmeticType_Div = 3,
    kArithmeticType_Mod = 4,
    kArithmeticType_ENUM_SIZE = 5  // Should always be the last one
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
    [[nodiscard]] ArithmeticType GetOpType() const;

    /**
     * @brief Get the deduced value of the arithmetic expression if both LHS and
     * RHS are deductible. Or std::nullopt otherwise.
     * @return ast::DeduceValue if deduced, std::nullopt if not.
     */
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
    ArithmeticType _type;
};

}  // namespace hzcc::ast
#endif  // HZCC_AST_EXPR_OP_OPERATOR_BASE_H
