#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "AST/ast_node.h"
#include "AST/function_node.h"
#include "AST/type/type.h"
#include "AST/value/variable.h"
#include "utils/Status.h"

#ifndef MYCC_AST_AST_H
#define MYCC_AST_AST_H

namespace Mycc::AST {
class ASTContext {
  public:
    ASTContext() = default;

    bool hasType(const std::string& basicString);

    std::shared_ptr<Type> getType(const std::string& name);

    void addASTNode(std::unique_ptr<ASTNode> node);

    Status addVariable(const Lexical::Token& name,
                       const std::shared_ptr<Type>& token_types,
                       const std::vector<Lexical::Token>& variable_attrs);

    Status setVariableInitialVal(
        const Lexical::Token& name,
        const std::unique_ptr<AST::ASTNode>& initial_val);

    Status leaveScope();

    void enterScope(std::string_view name);

    Status enterScope(std::string_view name, std::shared_ptr<Type> return_type);

  protected:
    class SymbolTable {
      public:
        explicit SymbolTable(std::shared_ptr<SymbolTable> parent = nullptr)
            : _upper_scope_table(std::move(parent)){};
        std::shared_ptr<SymbolTable> getUpperScope() {
            return _upper_scope_table;
        }

        bool hasType(std::string_view name) {
            if (_named_types.find(name) != _named_types.end()) {
                return true;
            } else {
                if (_upper_scope_table != nullptr) {
                    return _upper_scope_table->hasType(name);
                } else {
                    return false;
                }
            }
        }

        bool ExistsVariable(std::string_view name) {
            if (_variable_lookup_table.find(name) != _named_types.end()) {
                return true;
            } else {
                if (_upper_scope_table != nullptr) {
                    return _upper_scope_table->ExistsVariable(name);
                } else {
                    return false;
                }
            }
        }

        void AddStatement(std::unique_ptr<ASTNode> node) {
            _statements.push_back(std::move(node));
        }

      private:
        /**
         * Current Scoped Information
         */
        bool _is_function_scope = false;

        /**
         * Upper scope information
         */
        size_t _upper_scope_position = 0;
        std::shared_ptr<SymbolTable> _upper_scope_table;

        /**
         * Variables table
         */
        std::unordered_map<std::string_view, std::shared_ptr<Type>>
            _variable_lookup_table;

        /**
         * Types table
         */
        std::unordered_map<std::string_view, std::shared_ptr<Type>>
            _named_types;

        std::unordered_map<std::string_view, std::shared_ptr<SymbolTable>>
            _scoped_contexts;

        /**
         * Expressions list
         */
        std::list<std::unique_ptr<ASTNode>> _statements;
    };

  private:
    std::shared_ptr<SymbolTable> _current_context =
        std::make_shared<SymbolTable>();
    std::unordered_map<std::string_view, std::shared_ptr<SymbolTable>>
        _scoped_symbol_table;
};

}  // namespace Mycc::AST

#endif  // MYCC_AST_AST_H