//

// Created by Hanze Chen on 2022/2/3.

//

#include <memory>
#include <optional>
#include <string>

#include "ast/type/Type.h"
#include "macro.h"
#include "utils/logging.h"
#include "visitor.h"

#ifndef HZCC_AST_AST_NODE_H
#define HZCC_AST_AST_NODE_H
namespace hzcc::ast {
class Type;
class CastExpr;
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
    [[nodiscard]] const Position& Location() const;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] virtual bool HasBody() const { return false; }

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
     * @brief Print the debug information of the node will have same behavior as
     * Dump function but treat indent as empty string
     * @param out the output stream
     */
    std::ostream& operator<<(std::ostream& out) const;

    /**
     * @brief Get Name of the node
     * @return the name of the node
     */
    [[nodiscard]] const std::string_view& NodeName() const;

    /**
     * @brief Print the debug information of the node to the output stream
     * @param out the output stream
     * @param indent the indent of the node
     */
    void Dump(std::ostream& out, const std::string& ident) const;

    /**
     * @brief Interface for Visitor pattern
     * @param visitor  the Visitor
     * @return the result of the visit
     */
    [[nodiscard]] virtual Status visit(Visitor& visitor) = 0;

  protected:
    Stmt(Position loc, const std::string& name);

    /**
     * @brief An override function using print extra information when call
     * Stmt->Dump()
     * @param ident the current indentation level
     * @return std::ostream& the output stream
     */
    virtual void PrintDetail(std::ostream& out,
                             const std::string& ident) const = 0;

  private:
    const uint64_t _id;
    const Position _node_location;
    const std::string_view _node_name;

    inline static uint64_t s_id_counter = 0;
};

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

  protected:
    /**
     * @brief An override function using print extra information when call
     * Stmt->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;
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

    [[nodiscard]] std::list<std::unique_ptr<Stmt>>& GetBodyStatements();

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

  protected:
    /**
     * @brief An override function using print extra information when call
     * Stmt->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;
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

    [[nodiscard]] std::string_view DeclName() const;

    [[nodiscard]] virtual bool HasInitExpr() const;

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
    DoStmt(std::unique_ptr<Stmt> cond,  // NOLINT
           std::unique_ptr<Stmt> body,  // NOLINT
           const Position& pos);        // NOLINT

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
    [[nodiscard]] bool HasBody() const override { return true; }

    [[nodiscard]] std::unique_ptr<Stmt>& CondStmt();

    [[nodiscard]] std::unique_ptr<Stmt>& BodyStmt();

  protected:
    /**
     * @brief An override function using print extra information when call
     * Stmt->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;

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

  protected:
    /**
     * @brief An override function using print extra information when call
     * Stmt->Dump()
     * @param ident the current indentation level
     * @return std::ostream& the output stream
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;
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
    ForStmt(const Position& location, std::unique_ptr<Stmt> init,
            std::unique_ptr<Stmt> cond, std::unique_ptr<Stmt> step,
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
    [[nodiscard]] bool HasBody() const override { return true; }

    /**
     * @brief Get the init stmt.
     * @return The init stmt.
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& InitStmt();

    /**
     * @brief Get the condition stmt.
     * @return The condition stmt.
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& CondStmt();

    /**
     * @brief Get the step expression.
     * @return  The step expression.
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& StepStmt();

    /**
     * @brief Get the body of the for stmt
     * @return  The body of the for stmt
     */
    [[nodiscard]] const std::unique_ptr<Stmt>& BodyStmt();

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
    const std::unique_ptr<Stmt> _init;
    const std::unique_ptr<Stmt> _cond;
    const std::unique_ptr<Stmt> _step;
    const std::unique_ptr<Stmt> _body;
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

    bool set_else(std::unique_ptr<Stmt> Else);

    [[nodiscard]] const std::unique_ptr<Stmt>& CondStmt();

    [[nodiscard]] const std::unique_ptr<Stmt>& BodyStmt();

    [[nodiscard]] std::unique_ptr<Stmt>& ElseStmt();

    /**
     * @brief Get the list of else-if stmt. The return format is a vector
     * of pair, where the first element is the condition, and the second element
     * is the body.
     * @return The list of else-if stmt.
     */
    [[nodiscard]] std::vector<
        std::pair<std::unique_ptr<Stmt>, std::unique_ptr<Stmt>>>&
    ElseIfStmt();

    void addElseIf(std::unique_ptr<Stmt> Cond, std::unique_ptr<Stmt> Body);

    [[nodiscard]] bool HasElse() const;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool HasBody() const override;

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
    const std::unique_ptr<Stmt> _condition;
    const std::unique_ptr<Stmt> _if_body_statement;
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
    std::unique_ptr<Expr>& GetReturnVal();

    /**
     * @brief Determine whether the node is a return statement or not
     * @return true if the node is a return statement, false otherwise
     */
    [[nodiscard]] bool IsReturn() const override { return true; }

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
    std::unique_ptr<Expr> _return_val;
};

class WhileStmt : public Stmt {
  public:
    /**
     * @brief Constructor of WhileStmt
     * @param cond condition of the while
     * @param body body of the while
     * @param loc location of the while
     */
    WhileStmt(const Position& loc, std::unique_ptr<Stmt> cond,
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
    [[nodiscard]] bool HasBody() const override { return true; }

    [[nodiscard]] std::unique_ptr<Stmt>& CondStmt();

    [[nodiscard]] std::unique_ptr<Stmt>& BodyStmt();

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
    ParamVarDecl(const Position& loc,          // NOLINT
                 std::string_view decl_name,   // NOLINT
                 std::shared_ptr<Type> type);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

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
    const std::shared_ptr<Type> _type;
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
    FieldDecl(const Position& loc,          // NOLINT
              std::string_view name,        // NOLINT
              std::shared_ptr<Type> type);  // NOLINT

    /**
     * @brief Visitor for ast
     * @param visitor the ast Visitor
     * @return visit result
     */
    Status visit(Visitor& visitor) override;

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
    const std::shared_ptr<Type> _type;
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
               std::shared_ptr<StructType> type);  // NOLINT

    void add_field(std::unique_ptr<IDeclStmt> field);
    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    [[nodiscard]] bool IsStructDecl() const override { return true; }

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
    std::shared_ptr<StructType> _type;
    std::list<std::unique_ptr<IDeclStmt>> _fields;
};

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
    VarDecl(const Position& loc,          // NOLINT
            std::string_view name,        // NOLINT
            std::shared_ptr<Type> type,   // NOLINT
            std::unique_ptr<Expr> init);  // NOLINT

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
    [[nodiscard]] bool HasInitExpr() const override;

    /**
     * @brief Get the init ast Node of current VarDecl Node
     * @note will exit(-1) if current VarDecl doesn't contain init value when
     * compile as DEBUG mode
     * @return the init ast Node of current VarDecl Node
     */
    [[nodiscard]] std::unique_ptr<Expr>& GetInitExpr();

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
    std::unique_ptr<Expr> _init_expr;
    const std::shared_ptr<Type> _type;
};

class DeclStmt : public IDeclStmt {
  public:
    DeclStmt(const Position& loc,                             // NOLINT
             std::list<std::unique_ptr<VarDecl>> decl_list);  // NOLINT

    Status visit(Visitor& visitor) override;

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
    const std::shared_ptr<Type> _type;
    const std::list<std::unique_ptr<VarDecl>> _decl_list;
};

using ArgumentList =
    std::list<std::tuple<std::string, std::shared_ptr<ast::Type>, Position>>;

class FuncDeclStmt : public IDeclStmt {
  public:
    /**
     * @brief Construct a new Function Prototype For Function Node
     *
     * @param name the name of the function
     * @param return_type the return type of the function
     * @param loc the location of the function
     */
    FuncDeclStmt(const Position& loc,                 // NOLINT
                 std::string_view name,               // NOLINT
                 std::shared_ptr<Type> return_type);  // NOLINT

    /**
     * @brief ast Visitor acceptor
     * @param visitor Visitor object
     * @return return object of hzcc::Status with content of visit result
     */
    Status visit(Visitor& visitor) override;

    bool set_body(std::unique_ptr<ast::CompoundStmt> declaration);

    std::unique_ptr<ast::CompoundStmt>& Body();

    std::list<std::unique_ptr<ParamVarDecl>>& GetParams();

    ArgumentList getArguments();

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool HasBody() const override;

    [[nodiscard]] bool IsFuncDecl() const override;

    bool AddFunctionArgument(std::unique_ptr<ParamVarDecl> type);

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
    std::shared_ptr<Type> _return_type;
    std::unique_ptr<CompoundStmt> _function_body;
    std::list<std::unique_ptr<ParamVarDecl>> _function_param;
};

}  // namespace hzcc::ast

#endif  // HZCC_AST_AST_NODE_H
