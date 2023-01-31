#include <absl/container/flat_hash_map.h>

#include <list>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "SymbTbl.h"
#include "ast/Stmt.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "enums.h"
#include "utils/status/status.h"

#ifndef HZCC_AST_AST_H
#define HZCC_AST_AST_H
namespace hzcc::semantic {
class Ctx;
}

namespace hzcc::ast {
class SymbTbl;
class IDeclStmt;
class StructType;

class CompilationUnit {
    friend class hzcc::semantic::Ctx;

  public:
    explicit CompilationUnit(std::string file_name);

    CompilationUnit(CompilationUnit&&) = default;

    ~CompilationUnit();

    [[nodiscard]] std::string GetFileName() const;

    /**
     * ################################################################
     * #################   Scope related Functions  ###################
     * ################################################################
     */

    /**
     * @brief Leave the current scope
     */
    void leave_scope();

    /**
     * @brief Check if the current scope is the root scope
     * @return true if the current scope is the root scope
     */
    [[nodiscard]] bool at_root() const;

    /**
     * @brief Enter a new scope
     */
    void enter_scope(std::string_view name = "");

    /**
     * @brief Create a new function scope
     * @param name the name of the function
     * @param return_type the return type of the function
     * @return
     */
    void create_func(std::string_view name, const TypePtr& return_type);

    /**
     * @brief Get the return type of the current scope
     * @return the return type of the current function scope, or nearest
     * function scope if the current scope is not a function scope. std::nullopt
     * if at root scope
     */
    [[nodiscard]] std::optional<ast::TypePtr> ret_type() const;

    /**
     * ################################################################
     * #################   Type related Functions  ####################
     * ################################################################
     */
    bool has_type(std::string_view name);

    TypePtr get_type(std::string_view name,
                     std::list<Attribute> attr_list = {});

    StructTypePtr add_struct_type(std::string_view name,
                                  std::list<Attribute> attr);

    StructTypePtr get_struct_type(std::string_view name);

    bool has_var(const std::string& name, bool current_scope);

    /**
     * @brief Add a new Declare Node to the ast
     *
     * @Note: if the Declare Node has a duplicate name, it will be replaced the
     * old one
     * @param node
     * @return
     */
    void addDecl(std::unique_ptr<IDeclStmt> node);

    std::list<std::pair<std::string, std::unique_ptr<ast::IDeclStmt>>>&
    GetDecls();

    void new_scope(const std::string& name, const TypePtr& return_type);

    std::shared_ptr<ast::StructType> add_type(std::string_view name,
                                              std::list<Attribute>& attr_list);

    std::list<std::pair<std::string, std::unique_ptr<ast::IDeclStmt>>>
    get_decls();

  protected:
    const std::string _file_name;

    TypePtr search_type(std::string_view name);

  private:
    std::weak_ptr<SymbTbl> _current_context;

    /**
     * Global Types table
     */
    absl::flat_hash_map<std::string, TypePtr> _named_types;
    absl::flat_hash_map<std::string, std::shared_ptr<SymbTbl>>
        _scoped_symbol_table;

    std::list<std::pair<std::string, std::unique_ptr<ast::IDeclStmt>>>
        _global_decl;
};

}  // namespace hzcc::ast

#endif  // HZCC_AST_AST_H
