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

class CompilationUnit {
    friend class hzcc::semantic::Ctx;

  public:
    explicit CompilationUnit(std::string file_name);

    CompilationUnit(CompilationUnit&&) = default;

    ~CompilationUnit();

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
     * @brief Create a new function scope
     * @param name the to_str of the function
     * @param return_type the return type of the function
     * @return
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

    bool has_type(TokenType type);

    bool has_type(std::string_view name);


    QualTypePtr get_type(std::string_view name,
                         const std::vector<Qualifier>& qualifiers = {});

    QualTypePtr add_struct_type(std::string_view struct_name,
                                const std::vector<Qualifier>& attr_list = {});

    /** ---------------------------------------------------------------
     * ## Variable related Functions                              ###
     * -------------------------------------------------------------- */
    bool has_var(std::string_view name, bool current_scope);

    /** ---------------------------------------------------------------
     * ## Function related Functions                               ###
     * -------------------------------------------------------------- */

    bool has_func(std::string_view name);

    bool has_body(std::string_view name);

    QualTypePtr func_ret_type(std::string_view name);

    std::vector<QualTypePtr> func_param_types(std::string_view name);

    /**
     * @brief Add a new Declare Node to the ast
     *
     * @Note: if the Declare Node has a duplicate to_str, it will be replaced
     * the old one
     * @param node
     * @return
     */
    void addDecl(std::unique_ptr<IDeclStmt> node);

    /**
     * @brief decl node visitor
     * @detail visit all the decl node in the ast, and return the result of the
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
        Verifier verifier = [](auto) -> bool { return true; }) {
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
