//
// Created by Hanze Chen on 2022/2/3.
//

#include <stdint.h>  // for uint64_t, uintptr_t

#include <list>         // for list
#include <memory>       // for unique_ptr, shared_ptr, make_unique
#include <optional>     // for optional
#include <string>       // for string
#include <string_view>  // for string_view
#include <tuple>        // for tuple
#include <utility>      // for pair
#include <vector>       // for vector

#include "macro.h"                // for Position
#include "utils/status/status.h"  // for Status

#ifndef HZCC_AST_AST_NODE_H
#define HZCC_AST_AST_NODE_H
namespace hzcc::ast {
class Stmt;
class Type;
class Visitor;
class DeduceValue;

using StmtPtr = std::unique_ptr<Stmt>;

class Stmt {
  public:
    virtual ~Stmt() = default;

    /**
     * @brief The unique id of the node.
     * @return The unique id of the node.
     */
    [[nodiscard]] uintptr_t Id() const;

    /**
     * @brief Get the location of this Stmt in the source code. The location
     * have the form of (line, column).
     * @return The location of this Stmt
     */
    [[nodiscard]] const Position& loc() const;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] virtual bool has_body() const { return false; }

    /**
     * @brief Determine whether the node is a literal node or not
     * @return true if the node is a literal node, false otherwise
     */
    [[nodiscard]] virtual bool IsLiteral() const { return false; }

    /**
     * @brief Determine whether the current ast node is a declaration node or
     * not
     * @return true if the node is a declaration node, false otherwise
     */

    [[nodiscard]] virtual bool IsDeclNode() const { return false; }

    /**
     * @brief Determine whether the current ast node is a struct declaration
     * node or not node or not
     * @return true if the node is a struct declaration node, false otherwise
     */

    [[nodiscard]] virtual bool IsStructDecl() const { return false; }

    /**
     * @brief Determine whether the current ast node is a empty statement node
     * or not
     * @return true if the node is a empty statement node, false otherwise
     */
    [[nodiscard]] virtual bool IsEmptyStmt() const { return false; }

    /**
     * @brief Determine whether the node is a return statement or not
     * @return true if the node is a return statement, false otherwise
     */

    [[nodiscard]] virtual bool IsReturn() const { return false; }

    /**
     * @brief Get the unique name of the node in form of
     *         "[__NODE_NAME__:__NODE_ID__]"
     * @return The unique name of the node
     */
    [[nodiscard]] virtual std::string UniqueName() const;

    /**
     * @brief Get Name of the node
     * @return the name of the node
     */
    [[nodiscard]] std::string_view NodeName() const;

    /**
     * @brief Interface for Visitor pattern
     * @param visitor  the Visitor
     * @return the result of the visit
     */
    [[nodiscard]] virtual Status visit(Visitor& visitor) = 0;

  protected:
    Stmt(Position loc, std::string_view name);

  private:
    const uint64_t _id;
    const Position _node_location;
    const std::string_view _node_name;

    inline static uint64_t s_id_counter = 0;
};

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
    Expr(const Position& loc, std::string_view name) : Stmt(loc, name) {}
};

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
    TypeProxyExpr(const Position& loc,          // NO_LINT
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
     * @brief Get the type of the expression.
     * @return the type of the expression.
     */
    [[nodiscard]] std::shared_ptr<Type> retType() const override;

    [[nodiscard]] std::optional<DeduceValue> GetDeducedValue() const override;

  private:
    std::shared_ptr<Type> _type;
};

using TypeProxyExprPtr = std::unique_ptr<TypeProxyExpr>;

class BreakStmt : public Stmt {
  public:
    /**
     * @brief Constructor of BreakStmt
     * @param loc location of the break statement
     */
    explicit BreakStmt(const Position& loc);

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;
};

class CompoundStmt : public Stmt {
  public:
    /**
     * @brief Constructor of CompoundStmt
     * @param loc location of the compound statement
     */
    explicit CompoundStmt(const Position& loc);

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    void AddStatement(std::unique_ptr<Stmt> statement);

    [[nodiscard]] const std::unique_ptr<Stmt>& GetLastStatement() const;

    [[nodiscard]] std::list<std::unique_ptr<Stmt>>& get_body_stmts();

  private:
    std::list<std::unique_ptr<Stmt>> statements_{};
};

class ContinueStmt : public Stmt {
  public:
    /**
     * @brief Constructor of ContinueStmt
     * @param loc location of the continue statement
     */
    explicit ContinueStmt(const Position& loc);

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;
};

class IDeclStmt : public Stmt {
  public:
    /**
     * @brief Determine whether the current ast node is a declaration node or
     * not
     * @return true if the node is a declaration node, false otherwise
     */
    [[nodiscard]] bool IsDeclNode() const override;

    [[nodiscard]] virtual bool IsFuncDecl() const;

    [[nodiscard]] std::string_view decl_name() const;

    [[nodiscard]] virtual bool has_init() const;

    [[nodiscard]] virtual std::shared_ptr<Type> declType() const = 0;

  protected:
    /**
     * @brief Constructor of DeclStmt, this abstract class should never
     * initialized directly.
     * @param decl_name name of the declaration
     * @param location location of the declaration
     */
    IDeclStmt(const char* node_name,      // NOLINT
              std::string_view name,      // NOLINT
              const Position& location);  // NOLINT

  private:
    std::string _decl_name;
};

class DoStmt : public Stmt {
  public:
    /**
     * @brief Constructor of DoStmt
     * @param cond condition of the do statement
     * @param body body of the do statement
     * @param pos location of the do statement
     */
    DoStmt(const Position& pos,          // NOLINT
           std::unique_ptr<Stmt> cond,   // NOLINT
           std::unique_ptr<Stmt> body);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool has_body() const override { return true; }

    [[nodiscard]] std::unique_ptr<Stmt>& cond_stmt();

    [[nodiscard]] std::unique_ptr<Stmt>& body_stmt();

  protected:
    std::unique_ptr<Stmt> _cond;
    std::unique_ptr<Stmt> _body;
};

class EmptyStmt : public Stmt {
  public:
    /**
     * @brief Constructor of EmptyStmt
     */
    EmptyStmt();

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] bool IsEmptyStmt() const override;
};

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
    ForStmt(const Position& location);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool has_body() const override { return true; }

    void set_init(std::unique_ptr<Stmt> init);
    void set_cond(std::unique_ptr<Stmt> cond);
    void set_step(std::unique_ptr<Stmt> step);
    void set_body(std::unique_ptr<Stmt> body);
    void set_incr(std::unique_ptr<Stmt> incr);

    /**
     * @brief Get the init stmt.
     * @return The init stmt.
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& init_stmt();

    /**
     * @brief Get the condition stmt.
     * @return The condition stmt.
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& cond_stmt();

    /**
     * @brief Get the step expression.
     * @return  The step expression.
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& step_stmt();

    /**
     * @brief Get the body of the for stmt
     * @return  The body of the for stmt
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& body_stmt();

  private:
    const std::unique_ptr<Stmt> _init = std::make_unique<ast::EmptyStmt>();
    const std::unique_ptr<Stmt> _cond = std::make_unique<ast::EmptyStmt>();
    const std::unique_ptr<Stmt> _step = std::make_unique<ast::EmptyStmt>();
    const std::unique_ptr<Stmt> _body = std::make_unique<ast::EmptyStmt>();
};

class IfStmt : public Stmt {
  public:
    /**
     * @brief Constructor of IfStmt
     * @param cond condition of the if statement
     * @param body body of the if statement
     * @param location location of the if statement
     */
    IfStmt(const Position& location,     // NOLINT
           std::unique_ptr<Stmt> cond,   // NOLINT
           std::unique_ptr<Stmt> body);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    void set_else(std::unique_ptr<Stmt> else_body);

    [[nodiscard]] std::unique_ptr<Stmt>& cond_stmt();

    [[nodiscard]] std::unique_ptr<Stmt>& body_stmt();

    bool has_else() const;
    bool has_else_if() const;

    [[nodiscard]] std::unique_ptr<Stmt>& else_stmt();

    /**
     * @brief Get the list of else-if stmt. The return format is a vector
     * of pair, where the first element is the condition, and the second element
     * is the body.
     * @return The list of else-if stmt.
     */
    [[nodiscard]] std::vector<
        std::pair<std::unique_ptr<Stmt>, std::unique_ptr<Stmt>>>&
    else_if_stmts();

    void addElseIf(std::unique_ptr<Stmt> Cond, std::unique_ptr<Stmt> Body);

    [[nodiscard]] bool HasElse() const;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool has_body() const override;

  private:
    std::unique_ptr<Stmt> _condition;
    std::unique_ptr<Stmt> _if_body_statement;
    std::unique_ptr<Stmt> _else_statement_{nullptr};
    std::vector<std::pair<std::unique_ptr<Stmt>, std::unique_ptr<Stmt>>>
        _elseIfs;
};

class ReturnStmt : public Stmt {
  public:
    /**
     * @brief Constructor of ReturnStmt
     * @param expr expression of the return statement
     * @param loc location of the return statement
     */
    ReturnStmt(const Position& loc,          // NOLINT
               std::unique_ptr<Expr> expr);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Get the returned expression
     * @return The returned expression
     */
    std::unique_ptr<Expr>& ret_expr();

    /**
     * @brief Determine whether the node is a return statement or not
     * @return true if the node is a return statement, false otherwise
     */
    [[nodiscard]] bool IsReturn() const override { return true; }

  private:
    std::unique_ptr<Expr> _ret_expr;
};

class WhileStmt : public Stmt {
  public:
    /**
     * @brief Constructor of WhileStmt
     * @param cond condition of the while
     * @param body body of the while
     * @param loc location of the while
     */
    WhileStmt(const Position& loc,          // NOLINT
              std::unique_ptr<Stmt> cond,   // NOLINT
              std::unique_ptr<Stmt> body);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool has_body() const override { return true; }

    [[nodiscard]] std::unique_ptr<Stmt>& cond_stmt();

    [[nodiscard]] std::unique_ptr<Stmt>& body_stmt();

  protected:
    std::unique_ptr<Stmt> _cond;
    std::unique_ptr<Stmt> _body;
};

class ParamVarDecl : public IDeclStmt {
  public:
    /**
     * @brief Constructor of ParamVarDecl
     * @param decl_name name of the parameter
     * @param loc location of the parameter
     * @param type type of the parameter
     */
    ParamVarDecl(const Position& loc,                   // NOLINT
                 std::string_view decl_name,            // NOLINT
                 std::unique_ptr<TypeProxyExpr> type);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] std::unique_ptr<TypeProxyExpr>& type_expr();
    [[nodiscard]] std::shared_ptr<Type> declType() const override;

  private:
    std::unique_ptr<TypeProxyExpr> _type;
};
class FieldDecl : public IDeclStmt {
  public:
    /**
     * @brief Construct a new Var Decl object
     * @note this constructor will causing exit(-1) if type is nullptr when
     * compiling in debug mode
     * @param type The type of the variable
     * @param attrs The attributes of the variable
     * @param name The name of the variable
     */
    FieldDecl(const Position& loc,                   // NOLINT
              std::string_view name,                 // NOLINT
              std::unique_ptr<TypeProxyExpr> type);  // NOLINT

    /**
     * @brief Visitor for ast
     * @param visitor the ast Visitor
     * @return visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> declType() const override;

  private:
    std::unique_ptr<TypeProxyExpr> _type;
};

class RecordDecl : public IDeclStmt {
  public:
    /**
     * @brief Constructor of RecordDecl
     * @param decl_name name of the struct
     * @param loc loc of the struct
     * @param type type of the struct
     */
    RecordDecl(const Position& loc,  // NOLINT
               std::string_view decl_name,
               std::unique_ptr<TypeProxyExpr> type);  // NOLINT

    // todo sync to struct type
    void add_field(std::unique_ptr<IDeclStmt> field);
    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> declType() const override;

    [[nodiscard]] bool IsStructDecl() const override { return true; }

  private:
    std::unique_ptr<TypeProxyExpr> _type;
    std::list<std::unique_ptr<IDeclStmt>> _fields;
};

using RecordDEclPtr = std::unique_ptr<ast::RecordDecl>;

class VarDecl : public IDeclStmt {
  public:
    /**
     * @brief Construct a new Var Decl object
     * @note this constructor will causing exit(-1) if type is nullptr when
     * compiling in debug mode
     * @param type The type of the variable
     * @param attrs The attributes of the variable
     * @param name The name of the variable
     * @param init The initial value of the variable (optional)
     */
    VarDecl(const Position& loc,                   // NOLINT
            std::string_view name,                 // NOLINT
            std::unique_ptr<Expr> init,            // NOLINT
            std::unique_ptr<TypeProxyExpr> type);  // NOLINT

    /**
     * @brief Visitor for ast
     * @param visitor the ast Visitor
     * @return visit result
     */
    Status visit(Visitor& visitor) override;

    /**
     * @brief Inquire if current VarDecl contains init value or not
     * @return true if contains init value, false otherwise
     */
    [[nodiscard]] bool has_init() const override;

    /**
     * @brief Get the init ast Node of current VarDecl Node
     * @note will exit(-1) if current VarDecl doesn't contain init value when
     * compile as DEBUG mode
     * @return the init ast Node of current VarDecl Node
     */
    [[nodiscard]] std::unique_ptr<Expr>& init_expr();

    [[nodiscard]] std::shared_ptr<Type> declType() const override;

    [[nodiscard]] std::unique_ptr<TypeProxyExpr> type_expr();

  private:
    std::unique_ptr<Expr> _init_expr;
    std::unique_ptr<TypeProxyExpr> _type;
};

class DeclStmt : public IDeclStmt {
  public:
    DeclStmt(const Position& loc,                             // NOLINT
             std::list<std::unique_ptr<VarDecl>> decl_list);  // NOLINT

    Status visit(Visitor& visitor) override;

    [[nodiscard]] const std::list<std::unique_ptr<VarDecl>>& decl_list() {
        return _decl_list;
    }

    [[nodiscard]] std::shared_ptr<Type> declType() const override;

  private:
    const std::shared_ptr<Type> _type;
    const std::list<std::unique_ptr<VarDecl>> _decl_list;
};

using ArgumentList =
    std::list<std::tuple<std::string, std::shared_ptr<ast::Type>, Position>>;

class FuncDeclStmt : public IDeclStmt {
  public:
    friend class Visitor;

    /**
     * @brief Construct a new Function Prototype For Function Node
     *
     * @param name the name of the function
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

    ArgumentList getArguments();

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool has_body() const override;

    [[nodiscard]] bool IsFuncDecl() const override;

    bool AddFunctionArgument(std::unique_ptr<ParamVarDecl> type);

    [[nodiscard]] std::shared_ptr<Type> declType() const override;

    [[nodiscard]] std::unique_ptr<TypeProxyExpr> type_expr();

  private:
    std::unique_ptr<CompoundStmt> _func_body;
    std::unique_ptr<TypeProxyExpr> _ret_type;
    std::list<std::unique_ptr<ParamVarDecl>> _func_param;
};

}  // namespace hzcc::ast

#endif  // HZCC_AST_AST_NODE_H
