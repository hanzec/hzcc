//
// Created by chen_ on 2022/12/29.
//

#ifndef HZCC_SEMANTIC_H
#define HZCC_SEMANTIC_H
#include <absl/container/flat_hash_map.h>

#include "ast/CompilationUnit.h"
#include "ast/visitor.h"
namespace hzcc::semantic {
class Ctx {
  public:
    using TypePtr = std::shared_ptr<ast::Type>;
    using TokenList = std::list<Token>;

    explicit Ctx(std::shared_ptr<ast::CompilationUnit> compilationUnit);

    void leave_scope();

    void enter_scope();

    bool at_root() const;

    void enter_scope(std::string_view name);

    std::shared_ptr<ast::Type> scope_ret_type() const;

    // ################################################################
    // ###############    RetType related Functions     ###############
    // ################################################################

    /**
     * @brief Check if the type is already defined.
     * @details This function will first remove the potential keyword like
     * const, volatile, etc. to get the "pure" type name. If the "pure" type
     * name is a primitive type, it will return true. Otherwise, it will check
     * if current "pure" type name is already defined in the current scope, then
     * outer scope if not found, until the root scope.
     *
     * The Search order is:
     *      1. Global scope
     *      2. Current scope
     *      3. Outer scope
     *
     * @param basicString The name of the type.
     * @return True if the type is already defined. False otherwise.
     */
    bool hasType(std::string_view basicString);

    /**
     * @brief Get the type object by name.
     * @param name
     * @param attr_list
     * @return
     */
    TypePtr get_type(const std::string& name,                 // NOLINT
                     const std::list<Attribute>& attr_list);  // NOLINT

    TypePtr getArrayType(const TypePtr& base_type,         // NOLINT
                         const TokenList& attr_list,       // NOLINT
                         std::list<ast::ExprPtr>& shape);  // NOLINT

    TypePtr getFuncPtrType(const std::string& name);

    std::shared_ptr<ast::StructType> addStructType(const std::string& name,
                                                   const TokenList& attr_list);

    /**
     * ################################################################
     * ###############   Variable related Functions  ##################
     * ################################################################
     */
    bool has_var(const std::string_view& name, bool current_scope);

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

  protected:
    /**
     * @brief Search the user defined type by its name.
     * @details This function will search the user defined type by its name from
     * current scope to the root scope and the global scope lastly.
     *
     * The Search order is:
     *      1. Current scope
     *      2. Outer scope
     *      3. Global scope
     *
     * @param name The name of the type.
     * @return The type if found. nullptr otherwise.
     */
    TypePtr SearchNamedType(std::string_view name);

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
