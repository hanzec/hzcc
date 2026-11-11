//
// Created by chen_ on 2022/12/29.
//

#ifndef HZCC_SEMANTIC_H
#define HZCC_SEMANTIC_H
#include <absl/container/flat_hash_map.h>

#include "ast/CompilationUnit.h"
#include "ast/visitor.h"
#include "parser/common/Token.h"
namespace hzcc::semantic {
/**
 * @brief Legacy semantic-analysis view of a compilation unit and its active scope.
 * @note The constructor is declared but not implemented in this frontend snapshot.
 */
class Ctx {
  public:
    using TypePtr = std::shared_ptr<ast::Type>;
    using TokenList = std::list<Token>;

    /**
     * @brief Declare construction of a semantic context for a compilation unit.
     * @param compilationUnit Non-null unit whose symbol and scope state will be accessed.
     * @note This constructor currently has no implementation.
     */
    explicit Ctx(std::shared_ptr<ast::CompilationUnit> compilationUnit);

    /*
     * ----------------------------------------------------------------
     * ## Scope related Functions                                   ###
     * ----------------------------------------------------------------
     */

    /**
     * @brief Check if the current scope is the root scope.
     * @return true when the compilation unit reports the global scope.
     */
    [[nodiscard]] bool at_root() const;

    /**
     * @brief Leave the current scope
     */
    void leave_scope();

    /**
     * @brief Enter the scope
     */
    void enter_scope();

    /**
     * @brief Enter a named scope in the compilation unit.
     * @param name Name identifying the scope to enter.
     */
    void enter_scope(std::string_view name);

    /*
     *---------------------------------------------------------------
     * ## Variable related Functions                              ###
     * --------------------------------------------------------------
     */

    /**
     * @brief Check if the variable is defined in the current scope
     * @param name Variable identifier to look up.
     * @param current_scope  If true, only check the current scope, otherwise
     * check the whole scope
     * @return true if the variable is defined in the current scope or the whole
     * scope otherwise false
     */
    bool has_var(std::string_view name, bool current_scope);

    /**
     * @brief Forward a variable declaration to the active symbol table.
     * @param pos Source location of the declaration.
     * @param type Qualified type assigned to the variable.
     * @param name Variable identifier.
     * @pre A live non-root symbol table is selected in the compilation unit.
     */
    void add_var(Position pos,            // NOLINT
                 ast::QualTypePtr type,   // NOLINT
                 std::string_view name);  // NOLINT

    /**
     * @brief Look up the declaration location in the active scope chain.
     * @param name Identifier whose declaration is requested.
     * @return Pair with false as its first element and the declaration position, or (0, 0) when absent.
     */
    std::pair<bool, Position> var_def_pos(std::string_view name);

    /*
     * #############################################################
     * ###############   Function related Functions  ###############
     * #############################################################
     */
    /**
     * @brief Check whether the compilation unit contains a function declaration.
     * @param name Function identifier to look up.
     * @return True when the function table contains the name.
     */
    bool has_func(std::string_view name);

    /**
     * @brief Access the owning pointer for a known function declaration.
     * @param name Function identifier to look up.
     * @return Mutable reference to the entry in the compilation unit function table.
     * @pre The function exists; a missing name triggers a fatal diagnostic.
     */
    ast::FuncDeclStmtPtr& get_func(std::string_view name);

    /**
     * @brief Query the return type of the function associated with the active scope.
     * @return Qualified return type when available, otherwise std::nullopt.
     */
    [[nodiscard]] std::optional<ast::QualTypePtr> ret_type() const;

  private:
    std::list<TokenType> _attributes;
    std::shared_ptr<ast::CompilationUnit> _compilationUnit;
};

/**
 * @brief Legacy visitor that checks selected AST nodes and inserts permitted conversions.
 * @note This visitor is not wired into the command-line compilation pipeline.
 */
class analyzer : public ast::Visitor {
  public:
    /**
     * @brief Check the declared type, duplicate name, and initializer compatibility, then register the variable.
     * @param p_expr Non-null VarDecl node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::VarDecl* p_expr) override;

    /**
     * @brief Check a return expression against the current function return type.
     * @param p_expr Non-null ReturnStmt node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::ReturnStmt* p_expr) override;

    /**
     * @brief Check assignment operands and convert the right operand to the destination type when permitted.
     * @param p_expr Non-null AssignExpr node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::AssignExpr* p_expr) override;

    /**
     * @brief Visit conditional-expression branches and reconcile their result types.
     * @param p_expr Non-null TernaryExpr node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::TernaryExpr* p_expr) override;

    /**
     * @brief Check logical operands and reconcile their scalar types.
     * @param p_expr Non-null LogicalExpr node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::LogicalExpr* p_expr) override;

    /**
     * @brief Check bitwise operands and reconcile their operand types.
     * @param p_expr Non-null BitwiseExpr node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::BitwiseExpr* p_expr) override;

    /**
     * @brief Require the unary operand to be an assignable, non-const lvalue.
     * @param p_expr Non-null UnaryOperator node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::UnaryOperator* p_expr) override;

    /**
     * @brief Check arithmetic operand compatibility and apply permitted conversions.
     * @param p_expr Non-null ArithmeticExpr node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::ArithmeticExpr* p_expr) override;

    /**
     * @brief Check relational operand compatibility and reconcile operand types.
     * @param p_expr Non-null RelationalExpr node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::RelationalExpr* p_expr) override;

    /**
     * @brief Check the subscript base and index, converting the index where permitted.
     * @param p_expr Non-null ArraySubscriptExpr node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::ArraySubscriptExpr* p_expr) override;

    /**
     * @brief Check member access against the base record type and requested field.
     * @param p_expr Non-null MemberExpr node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::MemberExpr* p_expr) override;

    /**
     * @brief Check the function declaration, argument count, and argument compatibility.
     * @param p_expr Non-null FuncCallStmt node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::FuncCallStmt* p_expr) override;
    /**
     * @brief Visit every variable declaration in a declaration statement.
     * @param p_expr Non-null DeclStmt node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::DeclStmt* p_expr) override;

    /**
     * @brief Enter a defined function scope and check its parameters and body.
     * @param p_expr Non-null FuncDeclStmt node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::FuncDeclStmt* p_expr) override;

    /**
     * @brief Validate a parameter declaration and reject duplicate parameter names.
     * @param p_expr Non-null ParamVarDecl node being checked; child expressions may be updated.
     * @return OK when accepted, otherwise the first semantic error.
     */
    Status visit(hzcc::ast::ParamVarDecl* p_expr) override;

  private:
    Ctx _ctx;
};

/**
 * @brief Reserved entry point for semantic analysis of a compilation unit.
 * @param p_unit Unit intended to receive semantic checks.
 * @return Semantic-analysis status when an implementation is supplied.
 * @note This entry point is currently declared without an implementation.
 */
Status analyze(std::shared_ptr<ast::CompilationUnit> p_unit);

/* ---------------------------------------------------------------
 * ## Inlined Function Implementations                          ###
 *  --------------------------------------------------------------- */
ALWAYS_INLINE bool Ctx::at_root() const {
    return this->_compilationUnit->at_root();
}

ALWAYS_INLINE void Ctx::leave_scope() { this->_compilationUnit->leave_scope(); }

ALWAYS_INLINE void Ctx::enter_scope() { this->_compilationUnit->enter_scope(); }

ALWAYS_INLINE void Ctx::enter_scope(std::string_view name) {
    this->_compilationUnit->enter_scope(name);
}

ALWAYS_INLINE bool Ctx::has_var(std::string_view name, bool current_scope) {
    return this->_compilationUnit->has_var(name, current_scope);
}

ALWAYS_INLINE bool Ctx::has_func(std::string_view name) {
    return this->_compilationUnit->has_func(name);
}

ALWAYS_INLINE ast::FuncDeclStmtPtr& Ctx::get_func(std::string_view name) {
    LOG_IF(FATAL, !has_func(name)) << "Function " << name << " is not defined";
    return this->_compilationUnit->_func_tbl[name];
}

ALWAYS_INLINE std::optional<ast::QualTypePtr> Ctx::ret_type() const {
    return this->_compilationUnit->ret_type();
}

}  // namespace hzcc::semantic
#endif  // HZCC_SEMANTIC_H
