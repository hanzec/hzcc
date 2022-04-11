#ifndef MYCC_AST_AST_H
#define MYCC_AST_AST_H
#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "AST/ASTNode.h"
#include "AST/expr/decl_ref.h"
#include "AST/statement/function_decl.h"
#include "AST/type/Type.h"
#include "SymbolTable.h"
#include "lexical/Token.h"
#include "utils/Status.h"

namespace Mycc::AST {
class StructType;
class ASTContext {
  public:
    ASTContext() = default;

    ~ASTContext();

    std::string Dump() const;

    bool AtRoot();

    /**
     * #############################################################
     * ###############     AST related Functions     ###############
     * #############################################################
     */

    /**
     * @brief Add a new Declare Node to the AST
     *
     * @Note: if the Declare Node has a duplicate name, it will be replaced the
     * old one
     * @param node
     * @return
     */
    bool addDecl(std::unique_ptr<ASTNode> node);

    /**
     * #############################################################
     * ###############    Type related Functions     ###############
     * #############################################################
     */
    bool hasType(const std::string& basicString);

    std::shared_ptr<StructType> addStructType(
        const std::string& name, std::list<Lexical::TokenType>& attr_list);

    std::shared_ptr<Type> getType(
        const std::string& name,
        const std::list<Lexical::TokenType>& attr_list);

    std::shared_ptr<Type> getType(
        std::shared_ptr<Type> name,
        const std::list<std::shared_ptr<AST::Type>>& attrs);

    std::shared_ptr<Type> getType(
        const std::shared_ptr<AST::Type>& base_type,
        std::list<std::unique_ptr<AST::ASTNode>>& shape);

    /**
     * #############################################################
     * ###############   Variable related Functions  ###############
     * #############################################################
     */
    bool hasVariable(const std::string& name, bool sam_type = false);

    Status addVariable(const std::string& name,                // NOLINT
                       std::shared_ptr<Type>& variable_type);  // NOLINT

    std::shared_ptr<Type> getVariableType(const std::basic_string<char>& name);

    /**
     * #############################################################
     * ###############   Function related Functions  ###############
     * #############################################################
     */
    bool hasFunction(const std::string& name);

    bool addFunction(const std::string& name, std::shared_ptr<Type>& output,
                     const std::list<std::shared_ptr<Type>>& argument_list);

    bool hasFunctionBody(const std::string& name);

    std::tuple<std::shared_ptr<Type>, std::list<std::shared_ptr<Type>>>
    getFuncRetAndArgType(const std::basic_string<char>& name);

    /**
     * #############################################################
     * ###############  AST Scope related Functions  ###############
     * #############################################################
     */
    void leaveScope();

    void enterScope();

    void enterScope(const std::string& name,
                    const std::shared_ptr<Type>& return_type);

  protected:
    std::weak_ptr<SymbolTable> _current_context;

    /**
     * Function table
     */
    std::list<std::pair<std::string, std::unique_ptr<AST::DeclNode>>>
        _global_decl;

    /**
     * Global Types table
     */
    std::unordered_map<std::string, std::shared_ptr<Type>> _named_types;

    std::unordered_map<std::string, std::shared_ptr<SymbolTable>>
        _scoped_symbol_table;

    std::unordered_map<std::string, std::pair<std::shared_ptr<Type>,
                                              std::list<std::shared_ptr<Type>>>>
        _function_table;
};

}  // namespace Mycc::AST

#endif  // MYCC_AST_AST_H
