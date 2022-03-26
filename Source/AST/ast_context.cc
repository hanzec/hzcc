#include "ast_context.h"

#include "lexical/utils/symbol_utils.h"

namespace Mycc::AST {

bool ASTContext::hasType(const std::string &basicString) {
    // directly return true if the type is PrimitiveType
    if (Lexical::SymbolUtils::IsPrimitiveType(basicString.c_str())) {
        return true;
    } else {
        return _current_context->hasType(basicString);
    };
}
void ASTContext::addASTNode(std::unique_ptr<ASTNode> node) {
    _current_context->AddStatement(std::move(node));
}
std::shared_ptr<Type> ASTContext::getType(const std::string &name) {
    return {};
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
}  // namespace Mycc::AST