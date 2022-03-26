#include "ast_context.h"

#include "lexical/utils/symbol_utils.h"
#include "AST/type/array_type.h"
#include "AST/type/func_ptr_type.h"

namespace Mycc::AST {

bool ASTContext::hasType(const std::string &basicString) {
    // directly return true if the type is PrimitiveType
    if (Lexical::SymbolUtils::IsPrimitiveType(basicString.c_str())) {
        return true;
    } else {
        return _current_context->hasType(basicString);
    };
}
bool ASTContext::addASTNode(std::unique_ptr<ASTNode> node) {
    if(node == nullptr) {
        return false;
    }
    _current_context->AddStatement(std::move(node));
    return true;
}
std::shared_ptr<Type> ASTContext::getType(const std::string &name) {
    return std::make_shared<Type>(name);
}
Status ASTContext::leaveScope() {
    if (_current_context->getUpperScope() == nullptr) {
        return {Status::Code::INTERNAL, "Cannot leave scope"};
    } else {
        _current_context = _current_context->getUpperScope();
        return Status::OkStatus();
    }
}
void ASTContext::enterScope(std::string_view name) {
    if (_scoped_symbol_table.find(name) == _scoped_symbol_table.end()) {
        _scoped_symbol_table[name] =
            std::make_shared<SymbolTable>(_current_context);
    }

    _current_context = _scoped_symbol_table[name];
}
std::shared_ptr<Type> ASTContext::getArrayType(const std::string &name) {
    return std::make_shared<Type>(name);
}
std::shared_ptr<Type> ASTContext::getArrayType(
    const std::string &name, std::list<std::unique_ptr<AST::ASTNode>>& shape) {
    return std::make_shared<ArrayType>(name);
}
std::shared_ptr<Type> ASTContext::getFuncPtrType(
    const std::string &name,
    std::list<std::shared_ptr<AST::Type>> argument_list) {
    return std::make_shared<FuncPtrType>(name);
}
}  // namespace Mycc::AST