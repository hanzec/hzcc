//
// Created by chen_ on 2022/5/11.
//
#include "AST/CompilationUnit.h"
#include "AST/type/Type.h"
#include "SymbolTable.h"
#ifndef HZCC_SYNTAX_SYNTAX_CONTEXT_H
#define HZCC_SYNTAX_SYNTAX_CONTEXT_H
namespace Hzcc::Syntax {
class SyntaxContext {
  public:
    using TypePtr = AST::Type::TypePtr;

    explicit SyntaxContext(
        std::shared_ptr<AST::CompilationUnit> compilationUnit);

    void leaveScope();

    void enterScope();

    void enterScope(const std::string& name, const TypePtr& return_type);

    void addDecl(std::unique_ptr<AST::DeclStmt> type);

    /**
     * #############################################################
     * ###############    RetType related Functions     ###############
     * #############################################################
     */
    bool hasType(const std::string& basicString);

    TypePtr getNamedType(const std::string& name,
                         const std::list<Lexical::Token>& attr_list);

    TypePtr getArrayType(const TypePtr& base_type,
                         const std::list<Lexical::Token>& attr_list,
                         std::list<std::unique_ptr<AST::ASTNode>>& shape);

    TypePtr getFuncPtrType(const std::string& name);

    std::shared_ptr<AST::StructType> addStructType(
        const std::string& name, std::list<Lexical::Token>& attr_list);

    /**
     * #############################################################
     * ###############   Variable related Functions  ###############
     * #############################################################
     */
    bool hasVariable(const std::string& name, bool current_scope);

    void addVariable(int line_no, const std::string& name,  // NOLINT
                     TypePtr& variable_type);               // NOLINT

    std::pair<bool, int> getVariableInfo(const std::basic_string<char>& name);

    TypePtr getVariableType(const std::basic_string<char>& name);

    /**
     * #############################################################
     * ###############   Function related Functions  ###############
     * #############################################################
     */
    bool hasFunction(const std::string& name);

    bool addFunction(int line_no, const std::string& name, TypePtr& output,
                     const std::list<TypePtr>& argument_list);

    bool hasFunctionBody(const std::string& name);

    std::tuple<TypePtr, std::list<TypePtr>, int> getFuncRetAndArgType(
        const std::basic_string<char>& name);

    bool AtRoot() {
        return _current_context.lock()->getUpperScope() == nullptr;
    }

    TypePtr GetReturnType() {
        DLOG_ASSERT(_current_context.lock() != nullptr)
            << " should never call GetReturnType when in root context";
        return _current_context.lock()->GetReturnType();
    }

  private:
    std::list<Lexical::TokenType> _attributes;
    std::shared_ptr<AST::CompilationUnit> _compilationUnit;

    std::weak_ptr<SymbolTable> _current_context;

    /**
     * Global Types table
     */
    std::unordered_map<std::string, TypePtr> _named_types;
    std::unordered_map<std::string, std::shared_ptr<SymbolTable>>
        _scoped_symbol_table;

    /**
     * Function table:
     *  [name, [output_type, [argument_type, ...], line_no]]
     */
    std::unordered_map<std::string,
                       std::tuple<TypePtr, std::list<TypePtr>, int>>
        _function_def_table;
};
}  // namespace Hzcc::Syntax
#endif  // HZCC_SYNTAX_SYNTAX_CONTEXT_H
