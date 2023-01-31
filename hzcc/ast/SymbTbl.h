//
// Created by chen_ on 2022/4/9.
//
#include <absl/container/flat_hash_map.h>

#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "ast/type/Type.h"
#include "macro.h"
#ifndef HZCC_SOURCE_AST_SYMBOL_TABLE_H
#define HZCC_SOURCE_AST_SYMBOL_TABLE_H
namespace hzcc::ast {
class StructType;

class SymbTbl : public std::enable_shared_from_this<SymbTbl> {
  public:
    SymbTbl(TypePtr return_type, std::weak_ptr<SymbTbl> parent);

    /**
     * @brief Return the parent of this symbol table.
     * @note If this symbol table is the global symbol table, return nullptr.
     * @return std::shared_ptr<SymbTbl> The parent of this symbol table.
     */
    std::shared_ptr<SymbTbl> getUpperScope();

    /**
     * @brief Check if the symbol table has a symbol as type with the given
     * name.
     * @param name The name of the symbol.
     * @return true if type is existed.
     * @return false if type is not existed.
     */
    bool has_type(std::string_view name);

    /**
     * @brief Get the type's shared_ptr with the given name.
     *
     * @param name The name of the type.
     *
     * @details We are searching the symbol table from the current symbol table
     * first, if not found, then we will search the upper symbol table. Until
     * reaching root symbol table.
     *
     * @return std::shared_ptr<Type> The type with the given name or nullptr if
     * not found.
     */
    TypePtr get_type(std::string_view name);

    /**
     * @brief Register a type to the symbol table.
     * @param name The shared_ptr of the type to be registered.
     * @return true if type is successfully registered.
     * @return false if type name is existed as other type or as registered
     * variable identifier.
     */
    StructTypePtr add_struct_type(std::string_view name);

    /**
     * @brief Check if the symbol table has a variable with the given name.
     * @param name The name of the variable.
     * @param current_scope true when limit the search to the current scope.
     * @return true if variable is existed.
     * @return false if variable is not existed.
     */
    bool has_var(std::string_view name, bool current_scope);

    /**
     * @brief Register a variable to the symbol table.
     * @param pos the line number of the variable.
     * @param name the identifier of the variable to be registered.
     * @param token_types The shared_ptr of the variable types to be registered.
     * @return true if variable is successfully registered.
     * @return false if variable name is existed as other variable or as
     * registered type identifier.
     */
    void add_var(Position pos,                              // NOLINT
                 std::string_view name,                     // NOLINT
                 std::shared_ptr<ast::Type>& token_types);  // NOLINT

    int getVariableDeclLine(const std::string& name);

    /**
     * @brief Get the variable's shared_ptr with the given name.
     * @param name The name of the variable.
     * @return std::shared_ptr<type> The variable with the given name or
     * nullptr if not found.
     */
    std::shared_ptr<ast::Type> getVariableType(const std::string& name);

    /**
     * @brief Enter a new symbol table.
     * @return std::shared_ptr<SymbTbl> The new symbol table for the new
     * scope.
     */
    std::shared_ptr<SymbTbl> enter_scope();

    std::shared_ptr<ast::Type> ret_type();

  private:
    /**
     * Current Scoped Information
     */
    bool _is_function_scope = false;
    std::shared_ptr<ast::Type> _return_type;

    /**
     * Upper scope information
     */
    std::weak_ptr<SymbTbl> _upper_scope_table;

    /**
     * Variables table
     */
    absl::flat_hash_map<std::string, std::pair<Position, TypePtr>>
        _variable_lookup_table;

    /**
     * Types table
     */
    absl::flat_hash_map<std::string, TypePtr> _named_types;
    std::list<std::shared_ptr<SymbTbl>> _scoped_contexts;
};
}  // namespace hzcc::ast
#endif  // HZCC_SOURCE_AST_SYMBOL_TABLE_H
