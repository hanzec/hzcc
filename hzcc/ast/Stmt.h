//
// Created by Hanze Chen on 2022/2/3.
//

#include <cstdint>      // for uint64_t, uintptr_t
#include <list>         // for list
#include <memory>       // for unique_ptr, shared_ptr, make_unique
#include <optional>     // for optional
#include <string>       // for string
#include <string_view>  // for string_view
#include <tuple>        // for tuple
#include <utility>      // for pair
#include <vector>       // for vector

#include "ast/DeduceValue.h"
#include "enums.h"
#include "macro.h"                // for Position
#include "utils/status/status.h"  // for Status
#include "visitor.h"

#ifndef HZCC_AST_AST_NODE_H
#define HZCC_AST_AST_NODE_H
namespace hzcc::ast {
class Stmt;
class Expr;
class QualType;
class TypeProxyExpr;

using StmtPtr = std::unique_ptr<Stmt>;

/**
 * @brief The base class of all ast nodes. All ast nodes should inherit from
 * this class.
 * @details The Stmt class is the base class of all ast nodes. It provides
 *         interfaces for the Visitor pattern and the unique id of the node.
 *         All ast nodes should inherit from this class.
 *
 *         There are 6 kinds of Stmt which listed in following table:
 *            StmtType  |    Description  |     Enum Value    |  Example
 *         :-----------:|:---------------:|:-----------------:|:---------:
 *           ExprStmt   |  Expression     | StmtType::EXPR    | 1 + 2
 *           DeclStmt   |  Declaration    | StmtType::DECL    | int a = 1
 *           RetStmt    |  Return         | StmtType::RET     | return 1
 *          ControlStmt |  Control        | StmtType::CTRL    | break
 *           EmptyStmt  |  Empty          | StmtType::EMPTY   | ;
 *          DefaultStmt |  All other type | StmtType::DEFAULT | default
 *
 */
class Stmt {
  public:
    virtual ~Stmt() = default;

    /**
     * @brief The unique id of the node.
     * @return The unique id of the node.
     */
    [[nodiscard]] uintptr_t Id() const { return _id; }

    /**
     * @brief Get the location of this Stmt in the source code. The location
     * have the form of (line, column).
     * @return The location of this Stmt
     */
    [[nodiscard]] const Position& loc() const { return _node_location; }

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] virtual bool has_body() const { return false; }

    /**
     * @brief Get the statement type of the node
     * @return the statement type of the node
     */
    [[nodiscard]] StmtType stmt_type() const { return _stmt_type; };

    /**
     * @brief Get the unique to_str of the node in form of
     *         "[__NODE_NAME__:__NODE_ID__]"
     * @return The unique to_str of the node
     */
    [[nodiscard]] std::string UniqueName() const {
        return "[" + std::string(NodeName().data()) + ":" +
               std::to_string(Id()) + "]";
    }

    /**
     * @brief Get to_str of the node
     * @return the to_str of the node
     */
    [[nodiscard]] constexpr std::string_view NodeName() const {
        return _node_name;
    };

    /**
     * @brief Interface for Visitor pattern
     * @param visitor  the Visitor
     * @return the result of the visit
     */
    [[nodiscard]] virtual Status visit(Visitor& visitor) = 0;

  protected:
    /**
     * @brief Constructor of Stmt, **should only be called by derived class**
     * @param loc location of the statement
     * @param stmt_type type of the statement
     * @param name to_str of the statement node
     */
    template <int N>
    Stmt(Position loc, StmtType stmt_type, const char (&name)[N])
        : _node_name(name),
          _stmt_type(stmt_type),
          _id(s_id_counter += 1),
          _node_location(std::move(loc)) {
        // NOLINT
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(ERROR,
               !(_node_location.first >= 0 && _node_location.second >= 0))
            << "location invalid (should be greater than 0)";
        LOG_IF(ERROR, _node_name.empty()) << "node to_str empty";
#endif
    }

  private:
    const uint64_t _id;
    const StmtType _stmt_type;
    const Position _node_location;
    const std::string_view _node_name;
    inline static uint64_t s_id_counter = 0;
};

/**
 * @brief The base class of all expression nodes.
 */
class Expr : public Stmt {
  public:
    /**
     * @brief determine whether the node will return a location value (LValue)
     * or auto_factory value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */

    [[nodiscard]] virtual bool IsReturnLValue() const { return false; }

    [[nodiscard]] virtual std::optional<DeduceValue> GetDeducedValue() const {
        return std::nullopt;
    };

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
    [[nodiscard]] virtual std::shared_ptr<QualType> type() const = 0;

  protected:
    template <int N>
    Expr(const Position& loc, const char (&name)[N])
        : Stmt(loc, StmtType::EXPR, name) {}
};

using ExprPtr = std::unique_ptr<Expr>;

/**
 * @brief The class for type proxy expression. The type proxy expression is a
 * kind of expression that will make type checking stage easier.
 */
class TypeProxyExpr : public Expr {
  public:
    /**
     * @brief Construct a new TypeProxyExpr object. The TypeProxyExpr is
     * used to carry the type information. The type information is used to
     * determine the type of the expression.
     * @param type the type of the expression
     * @param expr the expression
     * @param loc location of the expression
     */
    TypeProxyExpr(const Position& loc,  // NO_LINT
                  std::shared_ptr<QualType> type)
        : Expr(loc, "TypeProxyExpr"), _type(std::move(type)) {}

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the type of the expression.
     * @return the type of the expression.
     */
    [[nodiscard]] std::shared_ptr<QualType> type() const override {
        return _type;
    }

  private:
    std::shared_ptr<QualType> _type;
};

using TypeProxyExprPtr = std::unique_ptr<TypeProxyExpr>;

/**
 * @brief The class for break statement
 */
class BreakStmt : public Stmt {
  public:
    /**
     * @brief Constructor of BreakStmt
     * @param loc location of the break statement
     */
    explicit BreakStmt(const Position& loc)
        : Stmt(loc, StmtType::DEFAULT, "BreakStmt"){};

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };
};

/**
 * @brief The class for compound statement
 */
class CompoundStmt : public Stmt {
  public:
    /**
     * @brief Constructor of CompoundStmt
     * @param loc location of the compound statement
     */
    explicit CompoundStmt(const Position& loc)
        : Stmt(loc, StmtType::DEFAULT, "CompoundStmt"){};

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Add a statement to the compound statement
     * @param statement statement to be added
     */
    void add_stmt(std::unique_ptr<Stmt> statement) {
        HZCC_RUNTIME_CHECK_BLOCK(
            { LOG_IF(FATAL, statement == nullptr) << "statement is nullptr"; });
        statements_.push_back(std::move(statement));
    };

    /**
     * @brief Get the last statement of the compound statement
     * @return the last statement of the compound statement
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& last_stmt() const {
        return statements_.back();
    };

    /**
     * @brief Get the body statements of the compound statement
     * @return the body statements of the compound statement
     */
    [[nodiscard]] const std::list<std::unique_ptr<Stmt>>& body_stmts() const {
        return statements_;
    };

  private:
    std::list<std::unique_ptr<Stmt>> statements_{};
};

/**
 * @brief The class for continue statement
 */
class ContinueStmt : public Stmt {
  public:
    /**
     * @brief Constructor of ContinueStmt
     * @param loc location of the continue statement
     */
    explicit ContinueStmt(const Position& loc)
        : Stmt(loc, StmtType::DEFAULT, "ContinueStmt"){};

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };
};

/**
 * @brief The base class for all declaration statement
 */
class IDeclStmt : public Stmt {
  public:
    /**
     * @brief The type of declaration
     * @return the type of declaration
     */
    [[nodiscard]] constexpr DeclType decl_type() const { return _decl_type; };

    /**
     * @brief Get the to_str of the declaration
     * @return the to_str of the declaration
     */
    [[nodiscard]] constexpr std::string_view name() const {
        return _decl_name;
    };

    /**
     * @brief Determine whether the declaration has an initializer or not
     * @return true if the declaration has initializer false otherwise
     */
    [[nodiscard]] virtual bool has_init() const { return false; };

    /**
     * @brief Get the type of the declaration
     * @return the type of the declaration
     */
    [[nodiscard]] virtual std::shared_ptr<QualType> type() const = 0;

  protected:
    /**
     * @brief Constructor of DeclStmt, this abstract class should never
     * initialized directly.
     * @param node_name to_str of the node
     * @param name to_str of the declaration
     * @param location location of the declaration
     */
    template <int N>
    IDeclStmt(DeclType _decl_type,         // NOLINT
              const char (&node_name)[N],  // NOLINT
              std::string_view name,       // NOLINT
              const Position& location)
        : Stmt(location, StmtType::DECL, node_name),
          _decl_name(name),
          _decl_type(_decl_type) {
        /** #####################################################################
         *  ### Runtime Assertion ###
         *  #####################################################################
         */
        HZCC_RUNTIME_CHECK_BLOCK({
            LOG_IF(FATAL, !_decl_name.empty())
                << this->UniqueName() << "decl to_str is empty";
        })
    }

  private:
    const DeclType _decl_type;
    const std::string _decl_name;
};

using IDeclStmtPtr = std::unique_ptr<IDeclStmt>;

/**
 * @brief The class for do-while statement
 */
class DoStmt : public Stmt {
  public:
    /**
     * @brief Constructor of DoStmt
     * @param cond condition of the do statement
     * @param body body of the do statement
     * @param pos location of the do statement
     */
    DoStmt(const Position& pos,         // NOLINT
           std::unique_ptr<Stmt> cond,  // NOLINT
           std::unique_ptr<Stmt> body)
        : Stmt(pos, StmtType::DEFAULT, "DoStmt"),
          _cond(std::move(cond)),
          _body(std::move(body)) {
        /** #################################################################
         *  ### Runtime Assertion ###
         *  ################################################################# */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, _body != nullptr) << UniqueName() << "body is nullptr";
        LOG_IF(FATAL, _cond != nullptr)
            << UniqueName() << "condition is nullptr";
#endif
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool has_body() const override { return true; }

    /**
     * @brief Get the condition of the do statement
     * @return the condition of the do statement
     */
    [[nodiscard]] std::unique_ptr<Stmt>& cond_stmt() { return _cond; };

    /**
     * @brief Get the body of the do statement
     * @return the body of the do statement
     */
    [[nodiscard]] std::unique_ptr<Stmt>& body_stmt() { return _body; };

  protected:
    std::unique_ptr<Stmt> _cond;
    std::unique_ptr<Stmt> _body;
};

/**
 * @brief The class for empty statement
 */
class EmptyStmt : public Stmt {
  public:
    /**
     * @brief Constructor of EmptyStmt
     */
    EmptyStmt() : Stmt({0, 0}, StmtType::EMPTY, "EmptyStmt"){};

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };
};

/**
 * @brief The class for for loop statement
 */
class ForStmt : public Stmt {
  public:
    /**
     * @brief Constructor of ForStmt
     * @param init initial expression of the for loop
     * @param cond condition of the for loop
     * @param step step expression of the for loop
     * @param body body of the for loop
     * @param location location of the for loop
     */
    explicit ForStmt(const Position& location)
        : Stmt(location, StmtType::CTRL, "ForStmt"){};

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool has_body() const override { return true; }

    /**
     * @brief Get the init stmt.
     * @return The init stmt.
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& init_stmt() { return _init; };

    /**
     * @brief Set the init stmt.
     * @param init The init stmt.
     */
    void set_init(std::unique_ptr<Stmt> init) {
        HZCC_RUNTIME_CHECK_BLOCK({
            LOG_IF(FATAL, init == nullptr)
                << UniqueName() << "init stmt is nullptr";
        })
        this->_init = std::move(init);
    }

    /**
     * @brief Get the condition stmt.
     * @return The condition stmt.
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& cond_stmt() { return _cond; };

    /**
     * @brief Set the condition stmt.
     * @param cond The condition stmt.
     */
    void set_cond(std::unique_ptr<Stmt> cond) {
        HZCC_RUNTIME_CHECK_BLOCK({
            LOG_IF(FATAL, cond == nullptr)
                << UniqueName() << "cond stmt is nullptr";
        })
        this->_cond = std::move(cond);
    }

    /**
     * @brief Get the step expression.
     * @return  The step expression.
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& incr_stmt() { return _incr; };

    /**
     * @brief Set the step expression.
     * @param incr The step expression.
     */
    void set_incr(std::unique_ptr<Stmt> incr) {
        HZCC_RUNTIME_CHECK_BLOCK({
            LOG_IF(FATAL, incr == nullptr)
                << UniqueName() << "incr stmt is nullptr";
        })
        this->_incr = std::move(incr);
    }

    /**
     * @brief Get the body of the for stmt
     * @return  The body of the for stmt
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& body_stmt() { return _body; };

    /**
     * @brief Set the body of the for stmt
     * @param body The body of the for stmt
     */
    void set_body(std::unique_ptr<Stmt> body) {
        HZCC_RUNTIME_CHECK_BLOCK({
            LOG_IF(FATAL, body == nullptr)
                << UniqueName() << "body stmt is nullptr";
        })
        this->_body = std::move(body);
    }

  private:
    std::unique_ptr<Stmt> _init = std::make_unique<ast::EmptyStmt>();
    std::unique_ptr<Stmt> _cond = std::make_unique<ast::EmptyStmt>();
    std::unique_ptr<Stmt> _incr = std::make_unique<ast::EmptyStmt>();
    std::unique_ptr<Stmt> _body = std::make_unique<ast::EmptyStmt>();
};

/**
 * @brief The class for if statement
 */
class IfStmt : public Stmt {
  public:
    /**
     * @brief Constructor of IfStmt
     * @param cond condition of the if statement
     * @param body body of the if statement
     * @param location location of the if statement
     */
    IfStmt(const Position& location,    // NOLINT
           std::unique_ptr<Stmt> cond,  // NOLINT
           std::unique_ptr<Stmt> body)
        : Stmt(location, StmtType::CTRL, "IfStmt"),
          _condition(std::move(cond)),
          _if_body_statement(std::move(body)) {
        /** #################################################################
         *  ### Runtime Assertion ###
         *  ################################################################# */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, body != nullptr) << UniqueName() << "body is nullptr";
        LOG_IF(FATAL, cond != nullptr)
            << UniqueName() << "condition is nullptr";
#endif
    };

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Determine whether the node have a else statement or not
     * @return true if the node have a else statement, false otherwise
     */
    [[nodiscard]] bool has_else() const { return _else_statement_ != nullptr; };

    /**
     * @brief get the else statement
     * @return the else statement
     */
    [[nodiscard]] std::unique_ptr<Stmt>& else_stmt() {
        return _else_statement_;
    };

    /**
     * @brief set the else statement
     * @param else_body the else statement
     */
    void set_else(std::unique_ptr<Stmt> else_body) {
        HZCC_RUNTIME_CHECK_BLOCK({
            LOG_IF(FATAL, else_body == nullptr)
                << UniqueName() << "else is nullptr";
        })
        _else_statement_ = std::move(else_body);
    }

    /**
     * @brief Get the condition stmt.
     * @return The condition stmt.
     */
    [[nodiscard]] std::unique_ptr<Stmt>& cond_stmt() { return _condition; };

    /**
     * @brief Get the body of the if stmt
     * @return  The body of the if stmt
     */
    [[nodiscard]] std::unique_ptr<Stmt>& body_stmt() {
        return _if_body_statement;
    };

    /**
     * @brief Determine whether the node have a else-if statement or not
     * @return true if the node have a else-if statement, false otherwise
     */
    [[nodiscard]] bool has_else_if() const { return !_else_ifs.empty(); };

    /**
     * @brief Get the list of else-if stmt. The return format is a vector
     * of pair, where the first element is the condition, and the second element
     * is the body.
     * @return The list of else-if stmt.
     */
    [[nodiscard]] std::vector<std::pair<StmtPtr, StmtPtr>>& else_if_stmts() {
        return _else_ifs;
    };

    /**
     * @brief Add an else-if statement to the node.
     * @param Cond The condition of the else-if statement.
     * @param Body The body of the else-if statement.
     */
    void add_else_if(std::unique_ptr<Stmt> Cond, std::unique_ptr<Stmt> Body) {
        HZCC_RUNTIME_CHECK_BLOCK({
            LOG_IF(FATAL, Cond == nullptr)
                << UniqueName() << "Cond statement is nullptr";
            LOG_IF(FATAL, Body == nullptr)
                << UniqueName() << "Body statement is nullptr";
        })
        _else_ifs.emplace_back(std::move(Cond), std::move(Body));
    }

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool has_body() const override { return true; };

  private:
    std::unique_ptr<Stmt> _condition;
    std::unique_ptr<Stmt> _if_body_statement;
    std::unique_ptr<Stmt> _else_statement_{nullptr};
    std::vector<std::pair<StmtPtr, StmtPtr>> _else_ifs;
};

/**
 * @brief The class for while statement
 */
class WhileStmt : public Stmt {
  public:
    /**
     * @brief Constructor of WhileStmt
     * @param cond condition of the while
     * @param body body of the while
     * @param loc location of the while
     */
    WhileStmt(const Position& loc,         // NOLINT
              std::unique_ptr<Stmt> cond,  // NOLINT
              std::unique_ptr<CompoundStmt> body)
        : Stmt(loc, StmtType::CTRL, "WhileStmt"),
          _cond(std::move(cond)),
          _body(std::move(body)) {
        /** ##################################################################
         *  ### Runtime Assertion ###
         *  ################################################################# */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, _body != nullptr) << UniqueName() << "body is nullptr";
        LOG_IF(FATAL, _cond != nullptr)
            << UniqueName() << "condition is nullptr";
#endif
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool has_body() const override { return true; }

    /**
     * @brief Get the condition stmt.
     * @return The condition stmt.
     */
    [[nodiscard]] std::unique_ptr<Stmt>& cond_stmt() { return _cond; };

    /**
     * @brief Get the body of the while stmt
     * @return  The body of the while stmt
     */
    [[nodiscard]] std::unique_ptr<CompoundStmt>& body_stmt() { return _body; };

  protected:
    std::unique_ptr<Stmt> _cond;
    std::unique_ptr<CompoundStmt> _body;
};

/**
 * @brief The class for param var declaration statement. A param var declaration
 * statement is a statement that declare a variable in the parameter list of a
 * function. It is not a real statement, but a part of the function declaration.
 * It is used to store the information of the parameter.
 */
class ParamVarDecl : public IDeclStmt {
  public:
    /**
     * @brief Constructor of ParamVarDecl
     * @param decl_name to_str of the parameter
     * @param loc location of the parameter
     * @param type type of the parameter
     */
    ParamVarDecl(const Position& loc,         // NOLINT
                 std::string_view decl_name,  // NOLINT
                 std::unique_ptr<TypeProxyExpr> type)
        : IDeclStmt(DeclType::PARAM, "ParamVarDecl", decl_name, loc),
          _type(std::move(type)) {
        /** ############################################################
         *  ### Runtime Assertion                                   ###
         *  ###########################################################*/
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, _type != nullptr) << UniqueName() << "type is nullptr";
#endif
    };

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the type proxy expr of the parameter
     * @return
     */
    [[nodiscard]] std::unique_ptr<TypeProxyExpr>& type_expr() { return _type; };

    /**
     * @brief Get the type of the parameter
     * @return  The type of the parameter
     */
    [[nodiscard]] std::shared_ptr<QualType> type() const override {
        return _type->type();
    };

  private:
    std::unique_ptr<TypeProxyExpr> _type;
};

/**
 * @brief The class for var declaration statement within the struct definition
 */
class FieldDecl : public IDeclStmt {
  public:
    /**
     * @brief Construct a new Var Decl object
     * @note this constructor will causing exit(-1) if type is nullptr when
     * compiling in debug mode
     * @param type The type of the variable
     * @param attrs The attributes of the variable
     * @param name The to_str of the variable
     */
    FieldDecl(const Position& loc,                  // NOLINT
              std::string_view name,                // NOLINT
              std::unique_ptr<TypeProxyExpr> type)  // NOLINT
        : IDeclStmt(DeclType::FIELD, "FieldDecl", name, loc),
          _type(std::move(type)) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, _type != nullptr) << UniqueName() << "type is nullptr";
#endif
    }

    /**
     * @brief Visitor for ast
     * @param visitor the ast Visitor
     * @return visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };
    ;

    /**
     * @brief Get the type proxy expr of the field
     * @return
     */
    [[nodiscard]] std::unique_ptr<TypeProxyExpr>& type_expr() { return _type; };

    /**
     * @brief Get the type of the field
     * @return  The type of the field
     */
    [[nodiscard]] std::shared_ptr<QualType> type() const override {
        return _type->type();
    };

  private:
    std::unique_ptr<TypeProxyExpr> _type;
};

/**
 * @brief The class for struct/union declaration statement
 */
class RecordDecl : public IDeclStmt {
  public:
    /**
     * @brief Constructor of RecordDecl
     * @param decl_name to_str of the struct
     * @param loc loc of the struct
     * @param type type of the struct
     */
    RecordDecl(const Position& loc,  // NOLINT
               std::string_view decl_name,
               std::unique_ptr<TypeProxyExpr> type);  // NOLINT

    /**
     * @brief Add a field to the struct
     * @param field  The field to be added
     */
    void add_field(std::unique_ptr<IDeclStmt> field);

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Get the type ptr of the struct
     * @return
     */
    [[nodiscard]] std::shared_ptr<QualType> type() const override {
        return _type->type();
    }

  private:
    std::unique_ptr<TypeProxyExpr> _type;
    std::list<std::unique_ptr<IDeclStmt>> _fields;
};

using RecordDEclPtr = std::unique_ptr<ast::RecordDecl>;

/**
 * @brief The class for var declaration statement
 */
class VarDecl : public IDeclStmt {
  public:
    /**
     * @brief Construct a new Var Decl object
     * @note this constructor will causing exit(-1) if type is nullptr when
     * compiling in debug mode
     * @param type The type of the variable
     * @param attrs The attributes of the variable
     * @param name The to_str of the variable
     * @param init The initial value of the variable (optional)
     */
    VarDecl(const Position& loc,         // NOLINT
            std::string_view name,       // NOLINT
            std::unique_ptr<Expr> init,  // NOLINT
            std::unique_ptr<TypeProxyExpr> type)
        : IDeclStmt(DeclType::VAR, "VarDecl", name, loc),
          _type(std::move(type)),
          _init_expr(std::move(init)) {
        /** #####################################################################
         *  ### Runtime Assertion ###
         *  #####################################################################
         */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        // to_str and type is checked in DeclStmt
        LOG_IF(FATAL, _init_expr != nullptr)
            << UniqueName() << "init expression is nullptr";
#endif
    }

    /**
     * @brief Visitor for ast
     * @param visitor the ast Visitor
     * @return visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Inquire if current VarDecl contains init value or not
     * @return true if contains init value, false otherwise
     */
    [[nodiscard]] bool has_init() const override {
        return _init_expr != nullptr;
    };

    /**
     * @brief Get the init ast Node of current VarDecl Node
     * @note will exit(-1) if current VarDecl doesn't contain init value
     * when compile as DEBUG mode
     * @return the init ast Node of current VarDecl Node
     */
    [[nodiscard]] std::unique_ptr<Expr>& init_expr() {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        DLOG_ASSERT(_init_expr != nullptr) << "init cast is nullptr";
#endif
        return _init_expr;
    };

    /**
     * @brief Get the type of the variable
     * @return  The type of the variable
     */
    [[nodiscard]] std::shared_ptr<QualType> type() const override {
        return _type->type();
    }

    /**
     * @brief Get the type proxy expr of the variable
     * @return The type proxy expr of the variable
     */
    [[nodiscard]] std::unique_ptr<TypeProxyExpr> type_expr() {
        return std::move(_type);
    };

  private:
    std::unique_ptr<Expr> _init_expr;
    std::unique_ptr<TypeProxyExpr> _type;
};

/**
 * @brief The combined declaration statement class, which contains a list of
 * VarDecl Node that are declared in the same line
 */
class DeclStmt : public IDeclStmt {
  public:
    DeclStmt(const Position& loc,  // NOLINT
             std::list<std::unique_ptr<VarDecl>> decl_list)
        : IDeclStmt(DeclType::COMB_VAR, "DeclStmt", "NO_NAME", loc),
          _decl_list(std::move(decl_list)) {
        /** #################################################################
         *  ### Runtime Assertion ###
         *  ################################################################# */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, _decl_list.empty())
            << UniqueName() << "decl list is empty";
#endif
    }

    /**
     * @brief Visitor for ast
     * @param visitor the ast Visitor
     * @return visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); }

    /**
     * @brief Get the decl list of the declaration statement
     * @return the decl list of the declaration statement
     */
    [[nodiscard]] const std::list<std::unique_ptr<VarDecl>>& decls() {
        return _decl_list;
    }

    /**
     * @brief Get the type of the declaration statement
     * @return the type of the declaration statement
     */
    [[nodiscard]] std::shared_ptr<QualType> type() const override {
        return _decl_list.front()->type();
    }

  private:
    const std::shared_ptr<QualType> _type;
    const std::list<std::unique_ptr<VarDecl>> _decl_list;
};

/**
 * @brief The class for function declaration statement
 */
class FuncDeclStmt : public IDeclStmt {
  public:
    friend class Visitor;

    /**
     * @brief Construct a new Function Prototype For Function Node
     *
     * @param name the to_str of the function
     * @param return_type the return type of the function
     * @param loc the location of the function
     */
    FuncDeclStmt(const Position& loc,                          // NOLINT
                 std::string_view name,                        // NOLINT
                 std::unique_ptr<TypeProxyExpr> return_type);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    bool set_body(std::unique_ptr<ast::CompoundStmt> declaration);

    std::unique_ptr<ast::CompoundStmt>& body_stmt();

    std::list<std::unique_ptr<ParamVarDecl>>& params();

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool has_body() const override;

    bool AddFunctionArgument(std::unique_ptr<ParamVarDecl> type);

    [[nodiscard]] std::shared_ptr<QualType> type() const override {
        return _ret_type->type();
    }

    [[nodiscard]] std::unique_ptr<TypeProxyExpr>& type_expr() {
        return _ret_type;
    }

  private:
    std::unique_ptr<CompoundStmt> _func_body;
    std::unique_ptr<TypeProxyExpr> _ret_type;
    std::list<std::unique_ptr<ParamVarDecl>> _func_param;
};

using FuncDeclStmtPtr = std::unique_ptr<FuncDeclStmt>;

class ReturnStmt : public Stmt {
  public:
    /**
     * @brief Constructor of ReturnStmt
     * @param expr expression of the return statement
     * @param loc location of the return statement
     */
    ReturnStmt(const Position& loc,  // NOLINT
               std::unique_ptr<Expr> expr)
        : Stmt(loc, StmtType::RETURN, "ReturnStmt"),
          _ret_expr(std::move(expr)) {
/** #####################################################################
 *  ### Runtime Assertion                                             ###
 *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG_IF(FATAL, _ret_expr != nullptr)
            << UniqueName() << "return statement is nullptr";
#endif
    }

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override { return visitor.visit(this); };

    /**
     * @brief Get the returned expression
     * @return The returned expression
     */
    std::unique_ptr<Expr>& ret_expr() { return _ret_expr; };

  private:
    std::unique_ptr<Expr> _ret_expr;
};

}  // namespace hzcc::ast

#endif  // HZCC_AST_AST_NODE_H
