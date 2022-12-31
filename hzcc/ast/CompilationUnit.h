#include <absl/container/flat_hash_map.h>

#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "SymbTbl.h"
#include "ast/Stmt.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "utils/status/status.h"

#ifndef HZCC_AST_AST_H
#define HZCC_AST_AST_H
namespace hzcc::ast {
class StructType;
class CompilationUnit {
  public:
    explicit CompilationUnit(std::string file_name);

    CompilationUnit(CompilationUnit&&) = default;

    ~CompilationUnit();

    [[nodiscard]] std::string GetFileName() const;

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

    bool has_type(std::string_view name);

    std::shared_ptr<ast::Type> get_type(std::string_view name);


    std::shared_ptr<ast::StructType> add_struct_type(std::string_view name, std::list<Attribute> attr);

    std::shared_ptr<ast::StructType> get_struct_type(std::string_view name);

    bool at_root() const;

    void leave_scope();

    void enter_scope();

    void new_scope(const std::string& name, const TypePtr& return_type);

    std::shared_ptr<ast::StructType> add_type(std::string_view name,
                                              std::list<Attribute>& attr_list);

    std::list<std::pair<std::string, std::unique_ptr<ast::IDeclStmt>>> get_decls();

  protected:
    const std::string _file_name;

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
