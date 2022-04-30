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
class CompilationUnit {
  public:
    int a;
    explicit CompilationUnit(std::string file_name);

    CompilationUnit(CompilationUnit &&) = default;

    ~CompilationUnit();

    std::string Dump() const;

    bool AtRoot();

    std::string GetFileName() const;
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
    void addDecl(std::unique_ptr<DeclNode> node);

    std::list<std::pair<std::string, std::unique_ptr<AST::DeclNode>>>& GetDecls();

    /**
     * #############################################################
     * ###############    Type related Functions     ###############
     * #############################################################
     */
    bool hasType(const std::string& basicString);

    std::shared_ptr<StructType> addStructType(
        const std::string& name, std::list<Lexical::TokenType>& attr_list);

    std::shared_ptr<Type> getNamedType(
        const std::string& name,
        const std::list<Lexical::TokenType>& attr_list);

    std::shared_ptr<Type> getArrayType(
        const std::shared_ptr<AST::Type>& base_type,
        std::list<std::unique_ptr<AST::ASTNode>>& shape);

    std::shared_ptr<Type> getFuncPtrType(
        std::shared_ptr<Type> name,
        const std::list<std::shared_ptr<AST::Type>>& attrs);
    /**
     * #############################################################
     * ###############   Variable related Functions  ###############
     * #############################################################
     */
    bool hasVariable(const std::string& name, bool current_scope);

    void addVariable(int line_no, const std::string& name,   // NOLINT
                     std::shared_ptr<Type>& variable_type);  // NOLINT

    std::pair<bool, int> getVariableInfo(const std::basic_string<char>& name);

    std::shared_ptr<Type> getVariableType(const std::basic_string<char>& name);

    /**
     * #############################################################
     * ###############   Function related Functions  ###############
     * #############################################################
     */
    bool hasFunction(const std::string& name);

    bool addFunction(int line_no, const std::string& name,
                     std::shared_ptr<Type>& output,
                     const std::list<std::shared_ptr<Type>>& argument_list);

    bool hasFunctionBody(const std::string& name);

    std::tuple<std::shared_ptr<Type>, std::list<std::shared_ptr<Type>>, int>
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

    std::shared_ptr<Type> GetReturnType();

  protected:
    const std::string _file_name;
    std::weak_ptr<SymbolTable> _current_context;

    std::list<std::pair<std::string, std::unique_ptr<AST::DeclNode>>>
        _global_decl;

    /**
     * Global Types table
     */
    std::unordered_map<std::string, std::shared_ptr<Type>> _named_types;

    std::unordered_map<std::string, std::shared_ptr<SymbolTable>>
        _scoped_symbol_table;

    /**
     * Function table
     */
    std::unordered_map<std::string,
                       std::tuple<std::shared_ptr<Type>,
                                  std::list<std::shared_ptr<Type>>, int>>
        _function_impl_table;
};

}  // namespace Mycc::AST

#endif  // MYCC_AST_AST_H
