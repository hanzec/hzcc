//
// Created by chen_ on 2022/5/11.
//
#ifndef HZCC_SYNTAX_SYNTAX_CONTEXT_H
#define HZCC_SYNTAX_SYNTAX_CONTEXT_H
#include <absl/container/flat_hash_map.h>

#include "SymbTbl.h"
#include "ast/CompilationUnit.h"
#include "ast/type/Type.h"
#include "parser/common/Token.h"
namespace hzcc::syntax {
class Ctx {
  public:
    using TypePtr = std::shared_ptr<ast::Type>;
    using TokenList = std::list<Token>;

    explicit Ctx(std::shared_ptr<ast::CompilationUnit> compilationUnit);

    void leave_scope();

    void enter_scope();

    void enterScope(const std::string& name, const TypePtr& return_type);

    void addDecl(std::unique_ptr<ast::IDeclStmt> type);

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
    TypePtr get_type(const std::string& name,      // NOLINT
                     const TokenList& attr_list);  // NOLINT

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
    bool hasVariable(const std::string& name, bool current_scope);

    void addVariable(Position pos,             // NOLINT
                     const std::string& name,  // NOLINT
                     TypePtr& variable_type);  // NOLINT

    std::pair<bool, int> getVariableInfo(const std::basic_string<char>& name);

    TypePtr getVariableType(const std::basic_string<char>& name);

    /**
     * #############################################################
     * ###############   Function related Functions  ###############
     * #############################################################
     */
    bool hasFunction(const std::string& name);

    bool addFunction(Position line_no, const std::string& name, TypePtr& output,
                     const std::list<TypePtr>& argument_list);

    bool hasFunctionBody(const std::string& name);

    std::tuple<TypePtr, std::list<TypePtr>, Position> getFuncRetAndArgType(
        const std::basic_string<char>& name);

    bool isRoot() {
        return _current_context.lock()->getUpperScope() == nullptr;
    }

    TypePtr GetReturnType() {
        DLOG_ASSERT(_current_context.lock() != nullptr)
            << " should never call GetReturnType when in root context";
        return _current_context.lock()->GetReturnType();
    }

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

    std::weak_ptr<SymbTbl> _current_context;

    /**
     * Global Types table
     */
    absl::flat_hash_map<std::string, TypePtr> _named_types;
    absl::flat_hash_map<std::string, std::shared_ptr<SymbTbl>>
        _scoped_symbol_table;

    /**
     * Function table:
     *  [name, [output_type, [argument_type, ...], line_no]]
     */
    absl::flat_hash_map<std::string,
                        std::tuple<TypePtr, std::list<TypePtr>, Position>>
        _function_def_table;
};
}  // namespace hzcc::syntax
#endif  // HZCC_SYNTAX_SYNTAX_CONTEXT_H
