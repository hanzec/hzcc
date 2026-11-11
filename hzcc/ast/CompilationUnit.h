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
#include "options.h"
#include "utils/status/status.h"
#include "utils/status/statusor.h"

#ifndef HZCC_AST_AST_H
#define HZCC_AST_AST_H
namespace hzcc::semantic {
class Ctx;
}

namespace hzcc::ast {
class SymbTbl;
class IDeclStmt;
class StructType;

/**
 * @brief Own the declarations and scoped symbol tables for one source file.
 */
class CompilationUnit {
    friend class hzcc::semantic::Ctx;

  public:
    /**
     * @brief Create a compilation unit and its root symbol table.
     * @param file_name Source filename copied into the unit.
     */
    explicit CompilationUnit(std::string file_name);

    /**
     * @brief Move symbol tables and declarations from another unit.
     * @note The source unit remains valid but its moved resources are unspecified.
     */
    CompilationUnit(CompilationUnit&&) = default;

    /**
     * @brief Release the owned declarations and scope tables for this source file.
     */
    ~CompilationUnit();

    /**
     * @brief Return the source filename associated with this unit.
     * @return An owning copy of the filename.
     */
    [[nodiscard]] std::string GetFileName() const;

    /** ---------------------------------------------------------------
     * #################   Scope related Functions  ###################
     *--------------------------------------------------------------- */

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
     * @brief Create and enter a named function scope beneath the root scope.
     * @param name Function name used to store the scope.
     * @param return_type Return type inherited by nested scopes.
     * @pre The current scope is the root scope.
     */
    void create_func(std::string_view name, const QualTypePtr& return_type);

    /**
     * @brief Get the return type of the current scope
     * @return the return type of the current function scope, or nearest
     * function scope if the current scope is not a function scope. std::nullopt
     * if at root scope
     */
    [[nodiscard]] std::optional<QualTypePtr> ret_type() const;

    /**---------------------------------------------------------------
     * #################   type related Functions  ####################
     *--------------------------------------------------------------- */

    /**
     * @brief Declare lookup of a type-specifier token in this unit.
     * @param type Token identifying a type specifier.
     * @return Whether the type is available.
     * @note This overload currently has no out-of-line implementation.
     */
    bool has_type(TokenType type);

    /**
     * @brief Check primitive or named-type availability in the current context.
     * @param name Type spelling to look up.
     * @return The result of the current primitive/global/local lookup policy.
     */
    bool has_type(std::string_view name);


    /**
     * @brief Resolve a primitive or named type in this compilation unit.
     * @param name Type spelling to look up.
     * @param qualifiers Qualifiers applied when constructing a local result wrapper.
     * @return A shared qualified type, or nullptr when lookup fails.
     * @note Lookup delegated to a parent symbol table retains that table's qualifier behavior.
     */
    QualTypePtr get_type(std::string_view name,
                         const std::vector<Qualifier>& qualifiers = {});

    /**
     * @brief Register a structure type in the current symbol table.
     * @param struct_name Structure tag without the struct prefix.
     * @param attr_list Qualifiers for the returned wrapper.
     * @return Shared ownership of the registered qualified type.
     * @pre The structure tag has not already been registered.
     */
    QualTypePtr add_struct_type(std::string_view struct_name,
                                const std::vector<Qualifier>& attr_list = {});

    /** ---------------------------------------------------------------
     * ## Variable related Functions                              ###
     * -------------------------------------------------------------- */
    /**
     * @brief Search for a variable from the active symbol table.
     * @param name Variable identifier.
     * @param current_scope Restrict lookup to the active scope when true.
     * @return true when a matching declaration is found.
     */
    bool has_var(std::string_view name, bool current_scope);

    /** ---------------------------------------------------------------
     * ## Function related Functions                               ###
     * -------------------------------------------------------------- */

    /**
     * @brief Check the function registry using the configured name-collision policy.
     * @param name Function identifier.
     * @return true for a registered function allowed by the variable-name policy.
     */
    bool has_func(std::string_view name);

    /**
     * @brief Check whether a named function has a definition.
     * @param name Function identifier to look up.
     * @return true if the function exists and owns a body.
     */
    bool has_body(std::string_view name);

    /**
     * @brief Look up a function's declared return type.
     * @param name Function identifier to look up.
     * @return A shared return type, or nullptr for an unknown function.
     */
    QualTypePtr func_ret_type(std::string_view name);

    /**
     * @brief Collect a function's parameter types in declaration order.
     * @param name Function identifier to look up.
     * @return Shared parameter types; an empty vector for an unknown or parameterless function.
     */
    std::vector<QualTypePtr> func_param_types(std::string_view name);

    /**
     * @brief Take ownership of a declaration and register it in the unit.
     * @param node Non-null declaration whose ownership is transferred here.
     * @note Currently only function declarations are registered. A later
     * definition supplies the body of an existing prototype; repeated
     * prototypes are ignored. Other declaration kinds are not stored.
     */
    void addDecl(std::unique_ptr<IDeclStmt> node);

    /**
     * @brief decl node visitor
     * @details visit all the decl node in the ast, and return the result of the
     * visit function in a list. If the verifier function return false, the
     * visitation will be stopped and the error will be returned immediately.
     * The default verifier function always return true.
     * @tparam Func the function type of the visit function
     * @tparam Verifier the function type of the verifier function
     * @param visit_function the visit function
     * @param verifier the verifier function
     * @return the list of return value of the visit function
     */
    template <
        typename Func, typename Verifier,
        typename FuncRet = std::invoke_result_t<Func, DeclStmt*>,
        std::enable_if_t<        // NOLINT
            std::conjunction_v<  // NOLINT
                std::is_invocable<Func, DeclStmt*>,
                std::is_invocable<Verifier, FuncRet>,
                std::is_same<bool, std::invoke_result_t<Verifier, FuncRet>>>,
            int> = 0>
    StatusOr<std::list<FuncRet>> visit_all_decl(
        Func visit_function,
        Verifier verifier = [](auto) -> bool {
            // The default verifier accepts every declaration visitor result.
            return true;
        }) {
        std::list<FuncRet> ret;
        for (auto& decl : _var_tbl) {
            auto visit_result = visit_function(decl.second.get());
            if (verifier(visit_result)) {
                ret.push_back(visit_result);
            } else {
                return visit_result;
            }
        }
        return ret;
    }

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

    absl::flat_hash_map<std::string, IDeclStmtPtr> _var_tbl;
    absl::flat_hash_map<std::string, FuncDeclStmtPtr> _func_tbl;
};

/** ---------------------------------------------------------------
 * ## Inlined Function Implementations                          ###
 *  --------------------------------------------------------------- */
ALWAYS_INLINE bool CompilationUnit::has_func(std::string_view name) {
    return _func_tbl.find(name) != _func_tbl.end() &&
           (Options::Global_allow_same_name_for_func_val ||
            !has_var(name, true));
};

ALWAYS_INLINE bool CompilationUnit::has_body(std::string_view name) {
    if (has_func(name)) {
        return _func_tbl[name]->has_body();
    }
    return false;
}

ALWAYS_INLINE QualTypePtr
CompilationUnit::func_ret_type(std::string_view name) {
    if (has_func(name)) {
        return _func_tbl[name]->type();
    }
    return nullptr;
}

}  // namespace hzcc::ast

#endif  // HZCC_AST_AST_H
