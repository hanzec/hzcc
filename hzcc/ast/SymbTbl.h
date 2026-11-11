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

/**
 * @brief Store declarations in one scope and retain nested scopes.
 * @details Child scopes are owned; the parent link is weak to avoid ownership cycles.
 */
class SymbTbl : public std::enable_shared_from_this<SymbTbl> {
  public:
    /**
     * @brief Create a scope with an inherited function return type.
     * @param return_type Shared return type; nullptr denotes a scope without a function.
     * @param parent Non-owning reference to the enclosing scope.
     */
    SymbTbl(QualTypePtr return_type,         // NOLINT
            std::weak_ptr<SymbTbl> parent);  // NOLINT

    /**
     * @brief Return the parent of this symbol table.
     * @note If this symbol table is the global symbol table, return nullptr.
     * @return std::shared_ptr<SymbTbl> The parent of this symbol table.
     */
    std::shared_ptr<SymbTbl> getUpperScope();

    /**
     * @brief Check if the symbol table has a symbol as type with the given
     * to_str.
     * @param name The to_str of the symbol.
     * @return true if type is existed.
     * @return false if type is not existed.
     */
    bool has_type(std::string_view name);

    /**
     * @brief Look up a named type from this scope through its parents.
     * @param name Registered type spelling, including any struct prefix.
     * @param qualifiers Qualifiers applied when the type is found in this scope.
     * @return A shared qualified wrapper, or nullptr if the name is unknown.
     * @note Parent-scope lookup currently uses the parent's default qualifier set.
     */
    QualTypePtr get_type(std::string_view name,
                         const std::vector<Qualifier>& qualifiers = {});

    /**
     * @brief Register a structure type under its struct-prefixed name.
     * @param name Structure tag copied into the new type.
     * @return Shared ownership of the registered structure type.
     * @pre No visible type already has this structure name.
     */
    StructTypePtr add_struct_type(std::string_view name);

    /**
     * @brief Check if the symbol table has a variable with the given to_str.
     * @param name The to_str of the variable.
     * @param current_scope true when limit the search to the current scope.
     * @return true if variable is existed.
     * @return false if variable is not existed.
     */
    bool has_var(std::string_view name, bool current_scope);

    /**
     * @brief Register a variable in this scope.
     * @param pos Source position of the declaration.
     * @param type Shared qualified variable type copied into the table.
     * @param name Variable identifier copied into the table.
     * @pre The variable does not duplicate a declaration in the current scope.
     */
    void add_var(Position pos,            // NOLINT
                 QualTypePtr& type,       // NOLINT
                 std::string_view name);  // NOLINT

    /**
     * @brief Find the source location of a visible variable declaration.
     * @param name Identifier to search from this scope through its parents.
     * @return The recorded declaration position.
     * @pre The variable exists in a visible scope.
     */
    Position var_def_pos(std::string_view name);
    /**
     * @brief Enter a new symbol table.
     * @return std::shared_ptr<SymbTbl> The new symbol table for the new
     * scope.
     */
    std::shared_ptr<SymbTbl> enter_scope();

    /**
     * @brief Return the function return type inherited by this scope.
     * @return Shared qualified type, or nullptr outside a function.
     */
    QualTypePtr ret_type();

  private:
    /**
     * Current Scoped Information
     */
    bool _is_function_scope = false;
    QualTypePtr _return_type;

    /**
     * Upper scope information
     */
    std::weak_ptr<SymbTbl> _upper_scope_table;

    /**
     * Variables table
     */
    absl::flat_hash_map<std::string, std::pair<Position, QualTypePtr>>
        _variable_lookup_table;

    /**
     * Types table
     */
    absl::flat_hash_map<std::string, TypePtr> _named_types;
    std::list<std::shared_ptr<SymbTbl>> _scoped_contexts;
};

/** ---------------------------------------------------------------
 * ## Inlined Function Implementations                          ###
 *  --------------------------------------------------------------- */

ALWAYS_INLINE QualTypePtr SymbTbl::ret_type() { return _return_type; }

}  // namespace hzcc::ast
#endif  // HZCC_SOURCE_AST_SYMBOL_TABLE_H
