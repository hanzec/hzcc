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
    [[nodiscard]] bool at_root() const {
        return this->_compilationUnit->at_root();
    };

    /**
     * @brief Leave the current scope
     */
    void leave_scope() { this->_compilationUnit->leave_scope(); };

    /**
     * @brief Enter the scope
     */
    void enter_scope() { this->_compilationUnit->enter_scope(); };

    /**
     * @brief Enter the scope with name
     * @param name  The name of the scope
     */
    void enter_scope(std::string_view name) {
        this->_compilationUnit->enter_scope(name);
    };

    /**
     *---------------------------------------------------------------
     * ## Variable related Functions                               ###
     * --------------------------------------------------------------
     */

    /**
     * @brief Check if the variable is defined in the current scope
     * @param name  The name of the variable
     * @param current_scope  If true, only check the current scope, otherwise
     * check the whole scope
     * @return true if the variable is defined in the current scope or the whole
     * scope otherwise false
     */
    bool has_var(std::string_view name, bool current_scope) {
        return this->_compilationUnit->has_var(name, current_scope);
    }

    void add_var(Position pos,            // NOLINT
                 TypePtr variable_type,   // NOLINT
                 std::string_view name);  // NOLINT

    std::pair<bool, Position> get_var_info(const std::string_view& name);

    TypePtr getVariableType(const std::basic_string<char>& name);

    /**
     * #############################################################
     * ###############   Function related Functions  ###############
     * #############################################################
     */
    bool has_func(std::string_view name);

    bool has_func_body(std::string_view name);

    bool add_func(Position line_no, std::string_view name, TypePtr output,
                  const std::list<TypePtr>& argument_list);

    std::tuple<TypePtr, std::list<TypePtr>, Position> func_def_info(
        std::string_view name);

    [[nodiscard]] std::optional<ast::TypePtr> ret_type() const {
        return this->_compilationUnit->ret_type();
    };

  private:
    std::list<TokenType> _attributes;
    std::shared_ptr<ast::CompilationUnit> _compilationUnit;

    /**
     * Function table:
     *  [name, [output_type, [argument_type, ...], line_no]]
     */
    absl::flat_hash_map<std::string,
                        std::tuple<TypePtr, std::list<TypePtr>, Position>>
        _function_def_table;
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
}  // namespace hzcc::semantic
#endif  // HZCC_SEMANTIC_H
