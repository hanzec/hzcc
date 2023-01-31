//
// Created by hanzech on 12/19/22.
//
#include "ast/Stmt.h"
#include "ast/type/Type.h"
#include "enums.h"

#ifndef HZCC_EXPR_H
#define HZCC_EXPR_H
namespace hzcc::ast {
/**
 * @brief Base class of all binary expression such as arithmetic, bitwise, etc.
 * @details This class is abstract, and should never be initialized directly.
 * Instead, it should be inherited by other classes. The type of the current
 * expression will be determined by the type of LHS and RHS. If both LHS and RHS
 * have the deduced type, then the final type will be same as LHS. If only LHS
 * has the deduced type or only RHS has the deduced type, then the returned type
 * will be same as the expression that can not be deduced. For example:
 *      1. int a = 1 + 2.0; // the type of the expression is double
 *      2. int a = 1 + 2; // the type of the expression is int
 *      3. int a = 1 + 2.0f; // the type of the expression is float
 *      4. int a = 1 + 2.0l; // the type of the expression is long double
 *
 * There are six kinds of binary expression:
 *     1. Arithmetic expression: +, -, *, /, %
 *     2. Bitwise expression: &, |, ^, ~, <<, >>
 *     3. Logical expression: &&, ||
 *     4. Relational expression: ==, !=, >, <, >=, <=
 *     5. Assignment expression: =, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
 *     6. Comma expression: ,
 */
class OperatorBase : public Expr {
  public:
    /**
     * @brief Get left hand side expression
     * @return left hand side expression
     */
    [[nodiscard]] std::unique_ptr<Expr>& lhs() { return _lhs; };

    /**
     * @brief Get the right hand side expression
     * @return right hand side expression
     */
    [[nodiscard]] std::unique_ptr<Expr>& rhs() { return _rhs; };

    /**
     * @brief Get the type of the current expression. The type of OperatorBase
     * and its inherited class will determine by left-hand-expression's and
     * right-hand-expression's deduced type:
     *      1. If both LHS and RHS have the deduced type, then the final type
     *       will be same as LHS
     *      2. If only LHS has the deduced type or only RHS has the deduced
     *      type, then the returned type will be same as the expression that can
     *      not be deduced.
     * @return return the type of the current expression
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        if (!_lhs->GetDeducedValue().has_value() &&
            !_rhs->GetDeducedValue().has_value()) {
            return _lhs->type();
        } else {
            return _lhs->GetDeducedValue().has_value() ? _rhs->type()
                                                       : _lhs->type();
        }
    };

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

    /**
     * @brief Get left hand side expression with const qualifier
     * @return left hand side expression with const qualifier
     */
    [[nodiscard]] const std::unique_ptr<Expr>& lhs_c() const { return _lhs; };

    /**
     * @brief Get the right hand side expression with const qualifier
     * @return right hand side expression with const qualifier
     */
    [[nodiscard]] const std::unique_ptr<Expr>& rhs_c() const { return _rhs; };

  private:
    std::unique_ptr<Expr> _lhs;
    std::unique_ptr<Expr> _rhs;
};

/**
 * @brief Base class of all cast expression such as ArrayToPointerCast, etc.
 * @details This class is abstract, and should never be initialized directly.
 * Instead, it should be inherited by other classes.
 *
 * There are five kinds of cast expression:
 *    1. ExplicitCastExpr: cast expression with explicit keyword
 *    2. ImplicitCastExpr: cast expression without explicit keyword
 *    3. LvalueToRvalueCast: cast expression from LValue to RValue
 *    4. IntegralCast: cast expression from integral type to integral type
 *    5. ArrayToPointerCast: cast expression from array to pointer
 */
class CastExpr : public Expr {
  public:
    /**
     * @brief Determine whether the node is a literal node or not
     * @return true if the node is a literal node, false otherwise
     */
    [[nodiscard]] bool IsLiteral() const override {
        return _expr->IsLiteral();
    };

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override {
        return _expr->IsReturnLValue();
    };

    /**
     * @brief return the expression that is being casted
     * @return the expression that is being casted
     */
    [[nodiscard]] const std::unique_ptr<Expr>& cast_expr() const {
        return _expr;
    };

    /**
     * @brief Get the deduced value of the cast expression. If the expression is
     * not a deductible, then return std::nullopt. The deduced value of cast is
     * exactly same as deduced value of the expression that is being casted.
     * @return
     */
    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override {
        return _expr->GetDeducedValue();
    };

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
    CastExpr(const char* node_name,     // NOLINT
             const Position& location,  // NOLINT
             std::unique_ptr<Expr> expr)
        : Expr(location, node_name), _expr(std::move(expr)) {
/** #####################################################################
 *  ### Runtime Assertion                                             ###
 *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, _expr != nullptr)
            << UniqueName() << "cast cast is nullptr";
#endif
    }

  private:
    const std::unique_ptr<Expr> _expr;
};

/**
 * @brief Represent a unary expression in AST tree.
 * @details Unary expression is an expression that only has one operand.
 * The following table is a list of unary operators:
 *          Operator  |    Description      |    Enum Value
 *        :----------:|:-------------------:|:-----------------:
 *              +     |    Unary plus       |    UnaryType::ADD
 *              -     |    Unary minus      |    UnaryType::SUB
 *              !     |    Logical not      |    UnaryType::NOT
 *              ~     |    Bitwise not      |    UnaryType::BIT_NOT
 *              &     |    Address of       |    UnaryType::ADDR
 *              *     |    Dereference      |    UnaryType::DEREF
 *          ++(expr)  |    Pre-increment    |    UnaryType::PRE_INC
 *          --(expr)  |    Pre-decrement    |    UnaryType::PRE_DEC
 *          (expr)++  |    Post-increment   |    UnaryType::POST_INC
 *          (expr)--  |    Post-decrement   |    UnaryType::POST_DEC
 *
 */
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
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the expression that is being operated
     * @return the expression that is being operated
     */
    [[nodiscard]] std::unique_ptr<Expr>& expr() { return _expr; };

    /**
     * @brief Get the type of the unary operator
     * @return the type of the unary operator
     */
    [[nodiscard]] UnaryType GetUnaryType() const { return _type; };

    /**
     * @brief Get the type of the expression
     * @return
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return _expr->type();
    };

  private:
    UnaryType _type;
    std::unique_ptr<Expr> _expr;
};

/**
 * @brief Represent a ternary expression in AST tree.
 * @details Ternary expression is a expression that has three parts, condition,
 * true expression and false expression. The condition part is a expression that
 * will be evaluated to a boolean value. If the condition is true, then the
 * true expression will be evaluated and return. Otherwise, the false expression
 * will be evaluated and return.
 *
 * The ternary expression is in the form of:
 *      condition ? true_expr : false_expr
 *
 * For example:
 *   int a = 1;
 *   int b = 2;
 *   int c = a > b ? a : b;
 *   // c will be 2
 *
 */
class TernaryExpr : public Expr {
  public:
    TernaryExpr(const Position& location,         // NOLINT
                std::unique_ptr<Expr> cond,       // NOLINT
                std::unique_ptr<Expr> true_expr,  // NOLINT
                std::unique_ptr<Expr> false_expr)
        : Expr(location, "TernaryExpr"),
          _cond(std::move(cond)),
          _true_expr(std::move(true_expr)),
          _false_expr(std::move(false_expr)) {
        /** #####################################################################
         * ### Runtime Assertion                                             ###
         * #####################################################################
         */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, _cond != nullptr)
            << this->UniqueName() << "cond is nullptr";
        INTERNAL_LOG_IF(FATAL, _true_expr != nullptr)
            << this->UniqueName() << "true_expr is nullptr";
        INTERNAL_LOG_IF(FATAL, _false_expr != nullptr)
            << this->UniqueName() << "false_expr is nullptr";
#endif
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the condition expression
     * @return the condition expression
     */
    [[nodiscard]] std::unique_ptr<Expr>& cond_expr() { return _cond; };

    /**
     * @brief Get the true expression
     * @return the true expression
     */
    [[nodiscard]] std::unique_ptr<Expr>& true_expr() { return _true_expr; };

    /**
     * @brief Get the false expression
     * @return the false expression
     */
    [[nodiscard]] std::unique_ptr<Expr>& false_expr() { return _false_expr; };

    /**
     * @brief Get the type of the ternary expression
     * @return the type of the ternary expression
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return _true_expr->type();
    };

    /**
     * @brief Get the deduced value of the ternary expression
     * @details If the condition expression is a constant expression, then the
     * deduced value of the ternary expression will be the deduced value of the
     * true expression or false expression. Otherwise, the deduced value will be
     * nullopt.
     * @return the deduced value of the ternary expression
     */
    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override {
        if (_cond->GetDeducedValue().has_value()) {
            if (_cond->GetDeducedValue().value().as_char()) {
                return _true_expr->GetDeducedValue();
            } else {
                return _false_expr->GetDeducedValue();
            }
        }
        return std::nullopt;
    };

  private:
    std::unique_ptr<Expr> _cond;
    std::unique_ptr<Expr> _true_expr;
    std::unique_ptr<Expr> _false_expr;
};

/**
 * @brief Represent a dummy expression node in AST tree.
 * @details This expression node is used to represent an empty expression. Such
 * expression does not have any effect on the program.Typically, this expression
 * is used at the case where the expression is optional such as empty return.
 */
class EmptyExpr : public Expr {
  public:
    EmptyExpr() : Expr(Position(), "EmptyExpr") {}

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return NoError(); };

    /**
     * @brief Return type of the SizeofExpr
     * @return will always return "int"
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return GetNumericalTypeOf<PrimitiveType::kVoid>();
    };
};

/**
 * @brief Represent a sizeof expression in AST tree.
 * @details Sizeof expression is a expression that returns the size of the
 * expression. The sizeof expression is in the form of: sizeof(expr)
 *
 * For example:
 *  int a = sizeof(int);
 *  // a will be 4
 */
class SizeofExpr : public Expr {
  public:
    /**
     * @brief Construct a new Sizeof Expr object
     * @param expr the expression to get the size of
     * @param location location of the sizeof operator
     */
    SizeofExpr(const Position& location,  // NOLINT
               std::unique_ptr<Stmt> expr)
        : Expr(location, "SizeofExpr"), _expr(std::move(expr)) {
/** #####################################################################
 *  ### Runtime Assertion                                             ###
 *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, _expr != nullptr)
            << UniqueName() << "cast is nullptr";
#endif
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Return type of the SizeofExpr
     * @return will always return "int"
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return GetNumericalTypeOf<PrimitiveType::kLong>();
    };

    /**
     * @brief Get the expression to get the size of
     * @return the expression to get the size of
     */
    [[nodiscard]] std::unique_ptr<Stmt>& expr() { return _expr; };

  private:
    std::unique_ptr<Stmt> _expr;
};

/**
 * @brief Represent a literal expression in AST tree.
 * @details Literal expression is a expression that represents a literal value.
 * There are four different types of literal expression in hzcc's
 * implementation:
 *   - Integer literal(LiteralType::Integer)
 *   - Real Number literal(LiteralType::Real_number)
 *   - Character literal(LiteralType::Char)
 *   - String literal(LiteralType::String)
 *
 * All literal expression will be automatically deducible to the corresponding
 * type.
 *
 */
class LiteralExpr : public Expr {
  public:
    /**
     * @brief Construct a new Literal Expr object from integer value
     * @note The LiteralTpe from this constructor is Integer
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
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Return type of the LiteralExpr
     * @return will return the type of the literal expression
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override;

    /**
     * @brief Determine whether the node is a literal node or not
     * @return true if the node is a literal node, false otherwise
     */
    [[nodiscard]] bool IsLiteral() const override { return true; };

    /**
     * @brief Get the Literal Type object
     * @return the literal type of the literal expression
     */
    [[nodiscard]] LiteralType literal_type() const { return _type; };

    /**
     * @brief Determine whether the literal expression is a return lvalue or not
     * @return false for literal expression since it is not a lvalue
     */
    [[nodiscard]] bool IsReturnLValue() const override { return false; };

    /**
     * @brief Get the Literal Value object
     * @return the literal value of the literal expression as a string
     */
    [[nodiscard]] std::string_view get_literal_val() const { return _value; };

    /**
     * @brief Get the Deduced Value object
     * @return the deduced value of the literal expression
     */
    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  private:
    std::string _value;
    enum LiteralType _type;
};

/**
 * @brief Represent a function call statement in AST tree.
 * @details Function call statement is a statement that calls a function.
 * The function call statement is in the form of: func_name(arg1, arg2, ...)
 * For example:
 * int a = func_name(1, 2, 3);
 *
 */
class FuncCallStmt : public Expr {
  public:
    /**
     * @brief Constructor of FuncCallStmt
     * @param type type of the function call
     * @param name name of the function call
     * @param loc location of the function call
     * @param args arguments of the function call
     */
    FuncCallStmt(const Position& loc,    // NOLINT
                 std::string_view name,  // NOLINT
                 std::list<std::unique_ptr<Expr>> args)
        : Expr(loc, "FuncCallStmt"),
          _func_args(std::move(args)),
          _name(std::string(name)) {
/** #####################################################################
 *  ### Runtime Assertion ###
 *  #####################################################################
 */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, _name.empty())
            << UniqueName() << "name is empty";
        INTERNAL_LOG_IF(FATAL, _ret_type != nullptr)
            << UniqueName() << "return type is nullptr";
        {
            uint_fast32_t args_count = 0;
            for (auto& arg : _func_args) {
                INTERNAL_LOG_IF(FATAL, arg != nullptr)
                    << UniqueName()
                    << "argument " + std::to_string(args_count) + " is nullptr";
            }
        }
#endif
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the function name of the function call statement
     * @return the function name of the function call statement
     */
    [[nodiscard]] std::string_view name() const { return _name; };

    /**
     * @brief Get the type of the function call statement
     * @return the type of the function call statement
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return _ret_type;
    }

    /**
     * @brief Get the arguments of the function call statement
     * @return the arguments of the function call statement
     */
    [[nodiscard]] std::list<ExprPtr>& args() { return _func_args; }

  private:
    TypePtr _ret_type;
    const std::string _name;
    std::list<ExprPtr> _func_args;
};

/**
 * @brief Represent a declaration reference expression in AST tree.
 * @details Declaration reference expression is a expression that refers to a
 * variable. The declaration reference expression is in the form of: var_name
 * For example:
 *  int a = 1;
 */
class DeclRefExpr : public Expr {
  public:
    /**
     * @brief Constructor of DeclRefExpr. If runtime assert is enabled, will
     * trigger an error if the given name is empty string.
     * @param loc location of the node
     * @param type type of the variable
     * @param name The name of the variable.
     */
    DeclRefExpr(const Position& loc,    // NO_LINT
                std::string_view name)  // NO_LINT
        : Expr(loc, "DeclRefExpr"), _name(std::string(name)) {
/** #####################################################################
 *  ### Runtime Assertion ###
 *  #####################################################################*/
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, _type != nullptr) << "type is nullptr";
        INTERNAL_LOG_IF(FATAL, !_name.empty()) << "name is empty string";
#endif
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief determine whether the node will return a location value
     * (LValue) or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override { return true; };

    /**
     * @brief get the name of the variable
     * @return the name of the variable
     */
    [[nodiscard]] std::string_view var_name() const { return _name; };

    /**
     * @brief get the type of the variable
     * @return the type of the variable
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return _type->type();
    };

  private:
    const std::string _name;
    const std::unique_ptr<TypeProxyExpr> _type;
};

/**
 * @brief Represent a array subscript expression in AST tree.
 * @details Array subscript expression is a expression that refers to a
 * element of an array. The array subscript expression is in the form of:
 * array_name[subscript]
 * For example:
 *  int a[10];
 */
class ArraySubscriptExpr : public Expr {
  public:
    /**
     * @brief ArraySubscriptExpr constructor. Create a new
     * ArraySubscriptExpr node. At runtime will check whether the array have
     * a type of array or not.
     * @param location location of the array subscript expression
     * @param array the array array expression
     * @param subscript the array subscript expression
     */
    ArraySubscriptExpr(const Position& location,     // NOLINT
                       std::unique_ptr<Expr> array,  // NOLINT
                       std::unique_ptr<Expr> subscript)
        : Expr(location, "ArraySubscriptExpr"),
          _array(std::move(array)),
          _subscript(std::move(subscript)) {
/** #####################################################################
 *  ### Runtime Assertion ###
 *  #####################################################################*/
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, _array != nullptr)
            << UniqueName() << "array is nullptr";
        INTERNAL_LOG_IF(FATAL, _subscript != nullptr)
            << UniqueName() << "subscript is nullptr";
        INTERNAL_LOG_IF(FATAL, _array->type()->is_arr())
            << UniqueName() << "not accessing array";
#endif
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief determine whether the node will return a location value
     * (LValue) or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override { return true; };

    /**
     * @brief determine whether the node is a dereference expression
     * @return true if the node is a dereference expression, false otherwise
     */
    [[nodiscard]] bool IsDereference() const override { return true; };

    /**
     * @brief get the type of the array subscript expression
     * @return the type of the array subscript expression
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return std::dynamic_pointer_cast<ast::ArrayType>(_array->type())
            ->GetBaseType();
    };

    /**
     * @brief get the array expression
     * @return the array expression
     */
    [[nodiscard]] std::unique_ptr<Expr>& base_expr() { return _array; };

    /**
     * @brief get the subscript expression
     * @return the subscript expression
     */
    [[nodiscard]] std::unique_ptr<Expr>& subscript_expr() {
        return _subscript;
    };

  private:
    std::unique_ptr<Expr> _array;
    std::unique_ptr<Expr> _subscript;
};

/**
 * @brief Represent a member access expression in AST tree.
 * @details Member access expression is a expression that refers to a
 * member of a struct. The member access expression is in the form of:
 * struct_name.member_name.
 *
 * For example:
 * struct A {
 *    int a;
 *    int b;
 *    int c;
 *    }
 *    A a;
 *    a.a = 1; // a is a struct, a.a is a member access expression
 *
 */
class MemberExpr : public Expr {
  public:
    /**
     * @brief Construct a new AccessExpr object. The AccessExpr is used to
     * access the struct member. Runtime will check the type is struct or
     * not if enabled. Also the field F must be not empty.
     * @param isPtr whether the access expression is a pointer access
     * @param field the field name
     * @param expr if have chain access or EmptyStmt otherwise
     * @param loc location of access expression
     */
    MemberExpr(bool isPtr,              // NO_LINT
               const Position& loc,     // NO_LINT
               std::string_view field,  // NO_LINT
               std::unique_ptr<Expr> expr)
        : Expr(loc, "AccessExpr"),
          _ptr_access(isPtr),
          _field(std::string(field)),
          _variable(std::move(expr)) {
/** #####################################################################
 *  ### Runtime Assertion ###
 *  #####################################################################*/
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, !field.empty())
            << UniqueName() << "field string empty";
        INTERNAL_LOG_IF(FATAL, _variable != nullptr)
            << UniqueName() << "chain_access is nullptr";
        INTERNAL_LOG_IF(FATAL, _variable->type()->is_struct())
            << UniqueName() << "chain_access is not struct";
#endif
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief determine whether the node will return a location value
     * (LValue) or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override { return true; };

    /**
     * @brief Get the type of specified field.
     * @return he type of specified field.
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return dynamic_cast<StructType*>(_variable->type().get())
            ->field_type(_field);
    };

    /**
     * @brief Get the field name.
     * @return the field name.
     */
    [[nodiscard]] std::string_view member_name() const { return _field; }

    /**
     * @brief Get the chain access expression.
     * @return the chain access expression.
     */
    [[nodiscard]] std::unique_ptr<ast::Expr>& base_expr() { return _variable; }

  private:
    std::string _field;
    const bool _ptr_access;
    std::unique_ptr<Expr> _variable;
};

/**
 * @brief Represent a relational expression in AST tree.
 * @details Relational expression is a expression that compares two
 * expressions. The relational expression is in the form of:
 * lhs operator rhs.
 *
 * Supported operators are listed in following table:
 *        Operator  |        Description       |     ENUM class
 *      :---------: | :----------------------: | :----------------:
 *           <      | less than                | RelationalType::LT
 *          ==      | equal to                 | RelationalType::EQ
 *          !=      | not equal to             | RelationalType::NE
 *           >      | greater than             | RelationalType::GT
 *          <=      | less than or equal to    | RelationalType::LE
 *          >=      | greater than or equal to | RelationalType::GE
 *
 * For example:
 * 1 < 2 // 1 and 2 are lhs and rhs, < is the operator
 *
 */
class RelationalExpr : public OperatorBase {
  public:
    /**
     * @brief Construct a new Relational Expr object. Supported operators
     * are listed in following table: Operator  | Description
     *      --------- | -----------
     *      <         | LT than
     *      ==        | Equal to
     *      !=        | Not equal to
     *      >         | Greater than
     *      <=        | LT than or equal to
     *      >=        | Greater than or equal to
     * Runtime assertion will be triggered if the operator is not supported
     * or type string has a length not equal to 2.
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
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the type of the relational expression
     * @return RelationalType of the expression
     */
    [[nodiscard]] RelationalType op_type() const { return _type; };

    /**
     * @brief Get the type of the current expression. The RelationalExpr
     * will always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of
     * char
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return GetNumericalTypeOf<PrimitiveType::kChar>();
    };

  private:
    RelationalType _type;
};

/**
 * @brief Represent a logical expression in AST tree.
 * @details Logical expression is a expression that compares two
 * expressions. The logical expression is in the form of:
 * lhs operator rhs.
 * Supported operators are listed in following table:
 *         Operator  |         Description        |     ENUM class
 *       :---------: | :------------------------: | :----------------:
 *           &&      | logical and                | LogicalType::AND
 *           ||      | logical or                 | LogicalType::OR
 */
class LogicalExpr : public OperatorBase {
  public:
    /**
     * @brief Construct a new Logical Expr object. Supported operators are
     * listed in following table:
     *      Operator   |  Description
     *     :---------: | :-----------:
     *         &&      |   Logical and
     *         ||      |   Logical or
     */
    LogicalExpr(const Position& loc,         // NOLINT
                std::string_view type,       // NOLINT
                std::unique_ptr<Expr> lhs,   // NOLINT
                std::unique_ptr<Expr> rhs);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get type of the logical expression
     * @return type of the logical expression
     */
    [[nodiscard]] LogicalType op_type() const { return _type; };

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of
     * char
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return GetNumericalTypeOf<PrimitiveType::kChar>();
    };

  private:
    LogicalType _type;
};

/**
 * @brief Represent a comma expression in AST tree.
 * @details Comma expression is a expression that compares two expressions.
 * The comma expression is in the form of:
 * lhs , rhs.
 */
class CommaExpr : public OperatorBase {
  public:
    /**
     * @brief CommaExpr constructor
     * @param lhs left hand side expression
     * @param rhs right hand side expression
     * @param loc location of comma operator
     */
    CommaExpr(const Position& loc,        // NOLINT
              std::unique_ptr<Expr> lhs,  // NOLINT
              std::unique_ptr<Expr> rhs)
        : OperatorBase(loc, "CommaExpr", std::move(rhs), std::move(lhs)) {}

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of
     * char
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return lhs_c()->type();
    }
};

/**
 * @brief Represent a bitwise expression in AST tree.
 * @details Bitwise expression is a expression that compares two
 * expressions. The bitwise expression is in the form of:
 * lhs operator rhs. Supported operators are listed in following table:
 *         Operator  |         Description        |     ENUM class
 *        :--------: | :------------------------: | :----------------:
 *           &       | bitwise and                | BitwiseType::AND
 *           |       | bitwise or                 | BitwiseType::OR
 *           ^       | bitwise xor                | BitwiseType::XOR
 *           <<      | left shift                 | BitwiseType::LSHIFT
 *           >>      | right shift                | BitwiseType::RSHIFT
 *
 */
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
     *   Runtime assertion will be triggered if the operator is not
     * supported. Or type string has a length > 3
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
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of
     * char
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return lhs_c()->type();
    }

    /**
     * @brief Get type of the bitwise expression
     * @return type of the bitwise expression
     */
    [[nodiscard]] BitwiseType op_type() const { return _type; };

  private:
    BitwiseType _type;
};

/**
 * @brief Represent a assign expression in AST tree.
 * @details Assign expression is a expression that compares two expressions.
 * The assign expression is in the form of: lhs operator rhs.
 *
 * Supported operators are listed in following table:
 *         Operator  |         Description        |     ENUM class
 *        :--------: | :------------------------: | :----------------:
 *            =      | assignment                 | AssignType::ASSIGN
 *            +=     | addition assignment        | AssignType::ADD
 *            -=     | subtraction assignment     | AssignType::SUB
 *            *=     | multiplication assignment  | AssignType::MUL
 *            /=     | division assignment        | AssignType::DIV
 *            %=     | modulus assignment         | AssignType::MOD
 *            <<=    | left shift assignment      | AssignType::LSHIFT
 *            >>=    | right shift assignment     | AssignType::RSHIFT
 *            &=     | bitwise and assignment     | AssignType::AND
 *            |=     | bitwise or assignment      | AssignType::OR
 *            ^=     | bitwise xor assignment     | AssignType::XOR
 */
class AssignExpr : public OperatorBase {
  public:
    /**
     * @brief Constructor for Assign Expression ast Node, supported
     * operators are listed in following table: Operator  | Description
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
     * Runtime assertion will be triggered if the operator is not supported.
     * Or type string has a length > 3
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
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the assign type
     * @return AssignType enum value
     */
    [[nodiscard]] AssignType op_type() const { return _type; };

    /**
     * @brief Get the type of the current expression. The LogicalExpr will
     * always return a char value.
     * @return will always return std::shared_ptr<Type> which have type of
     * char
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return lhs_c()->type();
    }

  private:
    AssignType _type;
};

/**
 * @brief Represent a arithmetic expression in AST tree.
 * @details Arithmetic expression is a expression that compares two
 * expressions. The arithmetic expression is in the form of: lhs operator rhs.
 * Supported operators are listed in following table:
 *         Operator  |         Description     |     ENUM class
 *        :--------: | :---------------------: | :----------------:
 *            +      | addition                | ArithmeticType::ADD
 *            -      | subtraction             | ArithmeticType::SUB
 *            *      | multiplication          | ArithmeticType::MUL
 *            /      | division                | ArithmeticType::DIV
 *            %      | modulus                 | ArithmeticType::MOD
 */

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
     * Runtime assertion will be triggered if the input symbol is not one of
     * the above. Or type is empty.
     * @param type arithmetic type in string format.
     * @param lhs left hand side of the expression.
     * @param rhs right hand side of the expression.
     * @param location location of the expression.
     */
    ArithmeticExpr(const Position& location,    // NOLINT
                   std::string_view type,       // NOLINT
                   std::unique_ptr<Expr> lhs,   // NOLINT
                   std::unique_ptr<Expr> rhs);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the type of the arithmetic expression.
     * @return Returned type of the arithmetic expression.
     */
    [[nodiscard]] ArithmeticType op_type() const { return _type; };

    /**
     * @brief Get the deduced value of the arithmetic expression if both LHS
     * and RHS are deductible. Or std::nullopt otherwise.
     * @return ast::DeduceValue if deduced, std::nullopt if not.
     */
    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  private:
    ArithmeticType _type;
};

/**
 * @brief Represent Explicit Cast Expression in AST tree.
 * @details Explicit cast expression is a expression that cast an expression
 * to a given type. The expression is in the form of: (type) expr.
 *
 * Example:
 *    (int) 1.0
 */
class ExplicitCastExpr : public CastExpr {
  public:
    /**
     * @brief CastExpr constructor. Will cast expression to the given
     * type. If runtime assert is enabled, will trigger an error if the
     * given type is same as the expression's type.
     * @param type the type to cast to
     * @param expr the expression to cast
     * @param loc location of the cast operator
     */
    ExplicitCastExpr(const Position& loc,         // NOLINT
                     std::unique_ptr<Expr> expr,  // NOLINT
                     std::unique_ptr<TypeProxyExpr> type)
        : CastExpr("ExplicitCastExpr", loc, std::move(expr)),
          _cast_type(std::move(type)) {
/** #####################################################################
 *  ### Runtime Assertion ###
 *  #####################################################################*/
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, _cast_type != nullptr)
            << UniqueName() << "cast type is nullptr";
        INTERNAL_LOG_IF(WARNING, cast_expr()->type() != _cast_type->type())
            << UniqueName() << "cast type is not equal to cast cast type";
#endif
    }

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return _cast_type->type();
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

  private:
    const std::unique_ptr<TypeProxyExpr> _cast_type;
};

class ImplicitCastExpr : public CastExpr {
  public:
    /**
     * @brief CastExpr constructor. Will cast expression to the given
     * to_type. If runtime assert is enabled, will trigger an error if the
     * given to_type is same as the expression's to_type.
     * @param to_type the to_type to cast to
     * @param expr the expression to cast
     * @param location location of the cast operator
     */
    ImplicitCastExpr(std::string name,          // NOLINT
                     const Position& location,  // NOLINT
                     std::unique_ptr<Expr> expr)
        : CastExpr("ImplicitCastExpr", location, std::move(expr)),
          _name(std::move(name)) {
/** #####################################################################
 *  ### Runtime Assertion ###
 *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, !_name.empty())
            << UniqueName() << "cast name is empty string";
        INTERNAL_LOG_IF(FATAL, cast_expr()->type() != nullptr)
            << UniqueName() << "cast cast have a nullptr type";
#endif
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

  private:
    const std::string _name;
};

/**
 * @brief Represent LvalueToRvalue Cast Expression in AST tree.
 * @details LvalueToRvalue cast expression is a expression that cast an
 * expression to a rvalue.
 */
class LvalueToRvalueCast : public ImplicitCastExpr {
  public:
    /**
     * @brief Construct a new LvalueToRvalue Cast. The expression has to be
     * an rvalue otherwise it will throw an runtime error.
     * @param location The location of the cast operator.
     * @param expr The expression to cast.
     */
    LvalueToRvalueCast(const Position& location,  // NOLINT
                       std::unique_ptr<Expr> expr)
        : ImplicitCastExpr("LvalueToRvalue", location, std::move(expr)) {
/** #####################################################################
 *  ### Runtime Assertion                                             ###
 *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, cast_expr()->IsReturnLValue())
            << "has to return left value";
#endif
    }

    /**
     * @brief determine whether the node will return a location value
     * (LValue) or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override { return false; };

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return cast_expr()->type();
    };

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };
};

/**
 * @brief Represent Integral Cast Expression in AST tree.
 * @details Integral cast expression is a expression that cast an expression
 * Cast an integral value to another value that have the same representation
 * size
 */
class IntegralCast : public ImplicitCastExpr {
  public:
    /**
     * @brief Construct a new Integral Cast.
     * @param location The location of the cast operator.
     * @param expr The expression to cast.
     * @param type The type to cast to.
     */
    IntegralCast(const Position& location,    // NOLINT
                 std::shared_ptr<Type> type,  // NOLINT
                 std::unique_ptr<Expr> expr)
        : ImplicitCastExpr("IntegralCast", location, std::move(expr)),
          _type(std::move(type)) {
        /** #####################################################################
         *  ### Runtime Assertion ###
         *  #####################################################################
         */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, _type != nullptr)
            << UniqueName() << "cast type is nullptr";
        INTERNAL_LOG_IF(WARNING, (*cast_expr()->type()) != *_type)
            << UniqueName() << "cast type is not equal to cast cast type";
        INTERNAL_LOG_IF(WARNING, (_type->is_numerical() ||
                                  cast_expr()->type()->is_numerical()))
            << UniqueName()
            << "cast type or expression type is not builtin type";
#endif
    }

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override { return _type; };

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

  private:
    std::shared_ptr<Type> _type;
};

/**
 * @brief Represent ArrayToPointerDecay Cast Expression in AST tree.
 * @details ArrayToPointerDecay cast expression is a expression that cast an
 * expression to a pointer. The expression has to be an array.
 */
class ArrayToPointerDecayCast : public ImplicitCastExpr {
  public:
    /**
     * @brief Construct a new ArrayToPointerDecay Cast. The expression has
     * to be an rvalue otherwise it will throw an runtime error.
     * @param location The location of the cast operator.
     * @param expr The expression to cast.
     */
    ArrayToPointerDecayCast(const Position& location,  // NOLINT
                            std::unique_ptr<Expr> expr)
        : ImplicitCastExpr("ArrayToPointerDecay", location, std::move(expr)) {
/** #####################################################################
 *  ### Runtime Assertion                                             ###
 *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        INTERNAL_LOG_IF(FATAL, cast_expr()->type()->is_ptr())
            << UniqueName() << "has to be a pointer type";

        INTERNAL_LOG_IF(FATAL, strcmp(cast_expr()->NodeName().data(),
                                      "ArraySubscriptExpr") == 0)
            << UniqueName() << "cast has to be a ArraySubscriptExpr";
#endif
    }

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> type() const override {
        return std::dynamic_pointer_cast<ArrayType>(cast_expr()->type())
            ->GetBaseType();
    };

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };
};
}  // namespace hzcc::ast
#endif  // HZCC_EXPR_H
