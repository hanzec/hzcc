#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "AST/ast_node.h"
#include "AST/expr/decl_ref.h"
#include "AST/statement/function_decl.h"
#include "AST/type/type.h"
#include "lexical/Token.h"
#include "utils/Status.h"
#ifndef MYCC_AST_AST_H
#define MYCC_AST_AST_H

namespace Mycc::AST {
class ASTContext {
  public:
    ASTContext() = default;

    std::string Dump() const;

    bool AtRot();

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

    bool addType(const std::string& name,
                 const std::shared_ptr<Type>& argument_list);

    std::shared_ptr<Type> getType(const std::string& name);

    std::shared_ptr<Type> getType(
        std::shared_ptr<Type> name,
        const std::list<std::shared_ptr<AST::Type>>& argument_list);

    std::shared_ptr<Type> getType(
        const std::shared_ptr<AST::Type>& base_type,
        std::list<std::unique_ptr<AST::ASTNode>>& shape);

    /**
     * #############################################################
     * ###############   Variable related Functions  ###############
     * #############################################################
     */
    bool hasVariable(const std::string& name, bool sam_type = false);

    Status addVariable(const std::string& name,              // NOLINT
                       std::shared_ptr<Type>& token_types);  // NOLINT

    std::shared_ptr<Type> getVariableType(const std::basic_string<char>& name);

    /**
     * #############################################################
     * ###############   Function related Functions  ###############
     * #############################################################
     */
    bool hasFunction(const std::string& name);

    bool hasFunctionBody(const std::string& name);

    bool addFunction(const std::string& name, std::shared_ptr<Type>& output,
                     const std::list<std::shared_ptr<Type>>& argument_list);

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
    class SymbolTable : public std::enable_shared_from_this<SymbolTable> {
      public:
        explicit SymbolTable(std::shared_ptr<Type> return_type = nullptr,
                             std::shared_ptr<SymbolTable> parent = nullptr)
            : _upper_scope_table(std::move(parent)),
              _return_type(std::move(return_type)){};

        std::shared_ptr<SymbolTable> getUpperScope() {
            return _upper_scope_table;
        }

        bool hasType(const std::string& name) {
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

        bool addType(const std::string& name,
                     const std::shared_ptr<Type>& new_type) {
            if (hasType(name)) {
                return false;
            } else {
                _named_types.insert(std::make_pair(name, new_type));
                return true;
            }
        }

        std::shared_ptr<Type> getType(const std::string& name) {
            if (!hasType(name)) return nullptr;
            if (_named_types.find(name) != _named_types.end()) {
                return _named_types[name];
            } else {
                if (_upper_scope_table != nullptr) {
                    return _upper_scope_table->getType(name);
                } else {
                    return nullptr;
                }
            }
        }

        bool hasVariable(const std::string& name) {
            if (_variable_lookup_table.find(name) !=
                _variable_lookup_table.end()) {
                return true;
            } else {
                if (_upper_scope_table != nullptr) {
                    return _upper_scope_table->hasVariable(name);
                } else {
                    return false;
                }
            }
        }

        bool addVariable(const std::string& name,
                         std::shared_ptr<Type>& token_types) {
            if (hasVariable(name) || token_types == nullptr) {
                return false;
            } else {
                _variable_lookup_table.insert(
                    std::make_pair(name, token_types));
                return true;
            }
        }

        std::shared_ptr<Type> getVariableType(const std::string& name) {
            if (_variable_lookup_table.find(name) !=
                _variable_lookup_table.end()) {
                return _variable_lookup_table[name];
            } else {
                if (_upper_scope_table != nullptr) {
                    return _upper_scope_table->getVariableType(name);
                } else {
                    return nullptr;
                }
            }
        }

        std::shared_ptr<SymbolTable> EnterScope() {
            auto new_scope =
                std::make_shared<SymbolTable>(_return_type, shared_from_this());
            _scoped_contexts.push_back(new_scope);
            return new_scope;
        }

      private:
        /**
         * Current Scoped Information
         */
        bool _is_function_scope = false;
        std::shared_ptr<Type> _return_type;

        /**
         * Upper scope information
         */
        std::shared_ptr<SymbolTable> _upper_scope_table;

        /**
         * Variables table
         */
        std::unordered_map<std::string, std::shared_ptr<Type>>
            _variable_lookup_table;

        /**
         * Types table
         */
        std::unordered_map<std::string, std::shared_ptr<Type>> _named_types;
        std::list<std::shared_ptr<SymbolTable>> _scoped_contexts;
    };

  protected:
    std::shared_ptr<SymbolTable> _current_context = nullptr;

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

    std::unordered_map<std::string,
                       std::pair<std::shared_ptr<Type>, std::list<std::shared_ptr<Type>>>>
        _function_table;
};

}  // namespace Mycc::AST

#endif  // MYCC_AST_AST_H