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
class Ctx {
  public:
    using TypePtr = std::shared_ptr<ast::Type>;
    using TokenList = std::list<Token>;

    explicit Ctx(std::shared_ptr<ast::CompilationUnit> compilationUnit);

    /**
     * ----------------------------------------------------------------
     * ## Scope related Functions                                   ###
     * ----------------------------------------------------------------
     */

    /**
     * @brief Check if the current scope is the root scope
     * @return
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
     * @brief Enter the scope with to_str
     * @param name  The to_str of the scope
     */
    void enter_scope(std::string_view name);

    /**
     *---------------------------------------------------------------
     * ## Variable related Functions                              ###
     * --------------------------------------------------------------
     */

    /**
     * @brief Check if the variable is defined in the current scope
     * @param name  The to_str of the variable
     * @param current_scope  If true, only check the current scope, otherwise
     * check the whole scope
     * @return true if the variable is defined in the current scope or the whole
     * scope otherwise false
     */
    bool has_var(std::string_view name, bool current_scope);

    void add_var(Position pos,            // NOLINT
                 ast::QualTypePtr type,   // NOLINT
                 std::string_view name);  // NOLINT

    std::pair<bool, Position> var_def_pos(std::string_view name);

    /**
     * #############################################################
     * ###############   Function related Functions  ###############
     * #############################################################
     */
    bool has_func(std::string_view name);

    ast::FuncDeclStmtPtr& get_func(std::string_view name);

    [[nodiscard]] std::optional<ast::QualTypePtr> ret_type() const;

  private:
    std::list<TokenType> _attributes;
    std::shared_ptr<ast::CompilationUnit> _compilationUnit;
};

class analyzer : public ast::Visitor {
  public:
    Status visit(hzcc::ast::VarDecl* p_expr) override;

    Status visit(hzcc::ast::ReturnStmt* p_expr) override;

    Status visit(hzcc::ast::AssignExpr* p_expr) override;

    Status visit(hzcc::ast::TernaryExpr* p_expr) override;

    Status visit(hzcc::ast::LogicalExpr* p_expr) override;

    Status visit(hzcc::ast::BitwiseExpr* p_expr) override;

    Status visit(hzcc::ast::UnaryOperator* p_expr) override;

    Status visit(hzcc::ast::ArithmeticExpr* p_expr) override;

    Status visit(hzcc::ast::RelationalExpr* p_expr) override;

    Status visit(hzcc::ast::ArraySubscriptExpr* p_expr) override;

    Status visit(hzcc::ast::MemberExpr* p_expr) override;

    Status visit(hzcc::ast::FuncCallStmt* p_expr) override;
    Status visit(hzcc::ast::DeclStmt* p_expr) override;

    Status visit(hzcc::ast::FuncDeclStmt* p_expr) override;

    Status visit(hzcc::ast::ParamVarDecl* p_expr) override;

  private:
    Ctx _ctx;
};

Status analyze(std::shared_ptr<ast::CompilationUnit> p_unit);

/** ---------------------------------------------------------------
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
    INTERNAL_LOG_IF(FATAL, !has_func(name))
        << "Function " << name << " is not defined";
    return this->_compilationUnit->_func_tbl[name];
}

ALWAYS_INLINE std::optional<ast::QualTypePtr> Ctx::ret_type() const {
    return this->_compilationUnit->ret_type();
}

}  // namespace hzcc::semantic
#endif  // HZCC_SEMANTIC_H
