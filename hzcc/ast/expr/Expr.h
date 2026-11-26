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
 * expression will be determined by the type of LHS and RHS. Arithmetic
 * expressions use the wider of their two numerical operand types.
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
     * @brief Get the type of the current expression.
     * @return The wider numerical operand type, or nullptr for an unresolved operand.
     */
    [[nodiscard]] QualTypePtr type() const override {
        const auto left = _lhs->type();
        const auto right = _rhs->type();
        if (!left || !right) return nullptr;
        if (left->is<TypeCategory::Numerical>() && right->is<TypeCategory::Numerical>() &&
            right->as<NumericalType>()->widening_rank() >
                left->as<NumericalType>()->widening_rank()) {
            return right;
        }
        return left;
    }

  protected:
    /**
     * @brief Own the two operands shared by binary-operator nodes.
     * @tparam N Extent of the node-name array.
     * @param loc Source position of the operator.
     * @param node_name Borrowed node label whose storage outlives this node.
     * @param lhs Non-null left operand whose ownership is transferred.
     * @param rhs Non-null right operand whose ownership is transferred.
     */
    template <int N>
    OperatorBase(const Position& loc,         // NOLINT
                 const char (&node_name)[N],  // NOLINT
                 std::unique_ptr<Expr> lhs,   // NOLINT
                 std::unique_ptr<Expr> rhs)
        : Expr(loc, node_name), _lhs(std::move(lhs)), _rhs(std::move(rhs)) {
/** #####################################################################
 *  ### Runtime Assertion                                             ###
 *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, _lhs == nullptr) << UniqueName() << "lhs is nullptr";
        LOG_IF(FATAL, _rhs == nullptr) << UniqueName() << "rhs is nullptr";
#endif
    }

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
     * @brief determine whether the node will return a location value (LValue)
     * or auto_factory value (RValue)
     * @return false because a cast produces an rvalue.
     */
    [[nodiscard]] bool IsReturnLValue() const override {
        return false;
    }

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
     * @return The operand's deduced value, or std::nullopt if it is unknown.
     * @note This base implementation does not apply numeric conversion to the
     * value; casts needing that behavior must override it.
     */
    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override {
        return _expr->GetDeducedValue();
    };

  protected:
    /**
     * @brief Initialize a cast node and take ownership of its operand.
     * @tparam N Extent of the borrowed node-name array.
     * @param node_name Node label whose storage outlives the cast.
     * @param location Source position of the cast.
     * @param expr Non-null expression whose ownership is transferred.
     */
    template <int N>
    CastExpr(const char (&node_name)[N],  // NOLINT
             const Position& location,    // NOLINT
             std::unique_ptr<Expr> expr)
        : Expr(location, node_name), _expr(std::move(expr)) {
/** #####################################################################
 *  ### Runtime Assertion                                             ###
 *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, _expr == nullptr)
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
    /**
     * @brief Decode a unary operator spelling and own its operand.
     * @param loc Source position of the operator.
     * @param type Unary operator spelling recognized by this constructor.
     * @param expr Non-null operand whose ownership is transferred here.
     */
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
     * @brief Return the result type of applying the unary operator to its operand.
     */
    [[nodiscard]] QualTypePtr type() const override {
        const auto operand = _expr->type();
        if (!operand) return nullptr;
        if (_type == UnaryType::ADDR) {
            return std::make_shared<QualType>(std::make_shared<PointerType>(operand));
        }
        if (_type == UnaryType::DEREF) {
            return operand->is<TypeCategory::Ptr>()
                       ? operand->as<PointerType>()->base_type()
                       : nullptr;
        }
        return _type == UnaryType::NOT ? GetNumericalTypeOf<PrimitiveType::kInt>()
                                       : operand;
    }

    /** @brief Dereferencing a pointer designates its target object. */
    [[nodiscard]] bool IsReturnLValue() const override {
        return _type == UnaryType::DEREF;
    }

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
    /**
     * @brief Construct a conditional expression from its three operands.
     * @param location Source position of the conditional operator.
     * @param cond Owned condition expression.
     * @param true_expr Owned expression for the true branch.
     * @param false_expr Owned expression for the false branch.
     */
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
        LOG_IF(FATAL, _cond == nullptr)
            << this->UniqueName() << "cond is nullptr";
        LOG_IF(FATAL, _true_expr == nullptr)
            << this->UniqueName() << "true_expr is nullptr";
        LOG_IF(FATAL, _false_expr == nullptr)
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
    [[nodiscard]] QualTypePtr type() const override {
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
        if (const auto condition = _cond->GetDeducedValue()) {
            if (condition->AsReal() != 0) {
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
    /**
     * @brief Construct a placeholder expression with a default source position.
     */
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
    [[nodiscard]] QualTypePtr type() const override {
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
        LOG_IF(FATAL, _expr == nullptr) << UniqueName() << "cast is nullptr";
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
    [[nodiscard]] QualTypePtr type() const override {
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
    [[nodiscard]] QualTypePtr type() const override;

    /**
     * @brief Get the Literal type object
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
     * @brief Construct a function-call expression with ordered arguments.
     * @param loc Source position of the call.
     * @param name Function identifier copied into the node.
     * @param args Argument expressions whose ownership is transferred.
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
        LOG_IF(FATAL, _name.empty()) << UniqueName() << "to_str is empty";
        {
            size_t args_count = 0;
            for (auto& arg : _func_args) {
                LOG_IF(FATAL, arg == nullptr)
                    << UniqueName()
                    << "argument " + std::to_string(args_count) + " is nullptr";
                ++args_count;
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
     * @brief Get the function to_str of the function call statement
     * @return the function to_str of the function call statement
     */
    [[nodiscard]] std::string_view name() const { return _name; };

    /**
     * @brief Get the type of the function call statement
     * @return The function return type, or nullptr before resolution.
     */
    [[nodiscard]] QualTypePtr type() const override { return _ret_type; }

    /** @brief Retain the return type from the function declaration resolved by the
     * visitor. */
    void set_return_type(QualTypePtr type) { _ret_type = std::move(type); }

    /**
     * @brief Get the arguments of the function call statement
     * @return the arguments of the function call statement
     */
    [[nodiscard]] std::list<ExprPtr>& params() { return _func_args; }

  private:
    QualTypePtr _ret_type;
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
     * @brief Create a reference to a named declaration.
     * @param loc Source position of the identifier use.
     * @param name Declaration identifier copied into the node.
     * @param type Optional shared type of the referenced declaration.
     */
    DeclRefExpr(const Position& loc,    // NO_LINT
                std::string_view name,  // NO_LINT
                QualTypePtr type = nullptr)
        : Expr(loc, "DeclRefExpr"), _name(name), _type(std::move(type)) {
/** #####################################################################
 *  ### Runtime Assertion ###
 *  #####################################################################*/
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, _name.empty()) << "to_str is empty string";
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
     * @brief get the to_str of the variable
     * @return the to_str of the variable
     */
    [[nodiscard]] std::string_view var_name() const { return _name; };

    /**
     * @brief get the type of the variable
     * @return the type of the variable
     */
    [[nodiscard]] QualTypePtr type() const override { return _type; };

  private:
    const std::string _name;
    const QualTypePtr _type;
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
        LOG_IF(FATAL, _array == nullptr) << UniqueName() << "array is nullptr";
        LOG_IF(FATAL, _subscript == nullptr)
            << UniqueName() << "subscript is nullptr";
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
    [[nodiscard]] QualTypePtr type() const override {
        const auto base = _array->type();
        if (base && base->is<TypeCategory::Array>())
            return base->as<ArrayType>()->GetBaseType();
        if (base && base->is<TypeCategory::Ptr>())
            return base->as<PointerType>()->base_type();
        return nullptr;
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
     * @param field the field to_str
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
        LOG_IF(FATAL, field.empty()) << UniqueName() << "field string empty";
        LOG_IF(FATAL, _variable == nullptr)
            << UniqueName() << "chain_access is nullptr";
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
    [[nodiscard]] QualTypePtr type() const override {
        const auto base = _variable->type();
        return base && base->is<TypeCategory::Struct>()
                   ? base->as<StructType>()->field_type(_field)
                   : nullptr;
    };

    /**
     * @brief Get the field to_str.
     * @return the field to_str.
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
     * @return will always return QualTypePtr which have type of
     * char
     */
    [[nodiscard]] QualTypePtr type() const override {
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
     * @return will always return QualTypePtr which have type of
     * char
     */
    [[nodiscard]] QualTypePtr type() const override {
        return GetNumericalTypeOf<PrimitiveType::kChar>();
    };

  private:
    LogicalType _type;
};

/**
 * @brief Represent a comma expression in AST tree.
 * @details The left operand is evaluated before the right operand supplies the result.
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
        : OperatorBase(loc, "CommaExpr", std::move(lhs), std::move(rhs)) {}

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Return the type of the right operand.
     */
    [[nodiscard]] QualTypePtr type() const override { return rhs_c()->type(); }
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
     * @brief Decode a bitwise operator and retain its operands.
     * @param loc Source position of the operator.
     * @param type Operator spelling supported by the bitwise parser.
     * @param left_expr Left operand whose ownership is transferred.
     * @param right_expr Right operand whose ownership is transferred.
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
     * @return will always return QualTypePtr which have type of
     * char
     */
    [[nodiscard]] QualTypePtr type() const override { return lhs_c()->type(); }

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
     * @brief Decode an assignment operator and retain its operands.
     * @param loc Source position of the operator.
     * @param type Assignment-operator spelling.
     * @param lhs Left operand whose ownership is transferred.
     * @param rhs Right operand whose ownership is transferred.
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
     * @return will always return QualTypePtr which have type of
     * char
     */
    [[nodiscard]] QualTypePtr type() const override { return lhs_c()->type(); }

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
     * type.
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
        LOG_IF(FATAL, _cast_type == nullptr)
            << UniqueName() << "cast type is nullptr";
#endif
    }

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] QualTypePtr type() const override {
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

/**
 * @brief Base node for a compiler-inserted conversion that owns its operand.
 */
class ImplicitCastExpr : public CastExpr {
  public:
    /**
     * @brief Construct the common wrapper for a compiler-inserted conversion.
     * @param name Conversion name copied into this node.
     * @param location Source position used for diagnostics.
     * @param expr Non-null operand whose ownership is transferred.
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
        LOG_IF(FATAL, _name.empty())
            << UniqueName() << "cast to_str is empty string";
        LOG_IF(FATAL, cast_expr()->type() == nullptr)
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
     * an lvalue.
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
        LOG_IF(FATAL, !cast_expr()->IsReturnLValue())
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
    [[nodiscard]] QualTypePtr type() const override {
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
    IntegralCast(const Position& location,  // NOLINT
                 QualTypePtr type,          // NOLINT
                 std::unique_ptr<Expr> expr)
        : ImplicitCastExpr("IntegralCast", location, std::move(expr)),
          _type(std::move(type)) {
        /** #####################################################################
         *  ### Runtime Assertion ###
         *  #####################################################################
         */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, _type == nullptr)
            << UniqueName() << "cast type is nullptr";
        LOG_IF(FATAL, !_type->is<TypeCategory::Numerical>() ||
                          !cast_expr()->type()->is<TypeCategory::Numerical>())
            << UniqueName() << "expected numerical cast types";
#endif
    }

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] QualTypePtr type() const override { return _type; };

    /** @brief Convert a constant operand to the destination numerical representation. */
    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override {
        const auto value = cast_expr()->GetDeducedValue();
        if (!value) return std::nullopt;
        if (_type->as<NumericalType>()->is_integer()) return DeduceValue(value->AsInt());
        return DeduceValue(value->AsReal());
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

  private:
    QualTypePtr _type;
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
     * to be an array.
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
        LOG_IF(FATAL, !cast_expr()->type()->is<TypeCategory::Array>())
            << UniqueName() << "expected an array operand";
#endif
    }

    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] QualTypePtr type() const override {
        return std::make_shared<QualType>(std::make_shared<PointerType>(
            cast_expr()->type()->as<ArrayType>()->GetBaseType()));
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
