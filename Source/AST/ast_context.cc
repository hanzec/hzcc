#include "ast_context.h"

#include <cassert>
#include <utility>

#include "AST/statement/compound.h"
#include "AST/type/array_type.h"
#include "AST/type/func_ptr_type.h"
#include "AST/utils/debug_utils.h"
#include "lexical/utils/symbol_utils.h"

namespace Mycc::AST {

ALWAYS_INLINE static std::string &ltrim(std::string &str) {
    auto it2 = std::find_if(str.begin(), str.end(), [](char ch) {
        return !std::isspace<char>(ch, std::locale::classic());
    });
    str.erase(str.begin(), it2);
    return str;
}

ALWAYS_INLINE static std::string &rtrim(std::string &str) {
    auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) {
        return !std::isspace<char>(ch, std::locale::classic());
    });
    str.erase(it1.base(), str.end());
    return str;
}

ALWAYS_INLINE static void replaceAll(std::string &str, const std::string &from,
                                     const std::string &to) {
    if (from.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();  // In case 'to' contains 'from', like
                                   // replacing 'x' with 'yx'
    }
}

bool ASTContext::hasType(const std::string &basicString) {
    if (basicString.empty()) {
        return false;
    }

    // looking for the raw type
    std::string name(basicString);
    for (const auto &type : Lexical::Keywords::kKeyword) {
        if (name.find(type) != std::string::npos &&
            strcmp(type, "struct") != 0 && strcmp(type, "union") != 0) {
            replaceAll(name, type, "");
        }
    }

    // trim the space
    name = ltrim(rtrim(name));

    // directly return true if the type is PrimitiveType
    if (Lexical::SymbolUtils::IsPrimitiveType(name.c_str())) {
        return true;
    } else {
        // we fist check our global scope
        if (_named_types.find(name) != _named_types.end()) {
            return true;
        } else {
            // then check the current scope
            if (_current_context != nullptr &&
                _current_context->hasType(basicString)) {
                return true;
            } else {
                return false;
            }
        }
    };
}

std::shared_ptr<Type> ASTContext::getType(const std::string &name) {
    if (_current_context != nullptr && _current_context->hasType(name)) {
        return _current_context->getType(name);
    } else {
        if (_named_types.find(name) != _named_types.end()) {
            return _named_types[name];
        } else {
            // looking for the raw type
            std::string raw_type(name);
            std::list<std::string> attr_list;
            for (const auto &type : Lexical::Keywords::kKeyword) {
                if (raw_type.find(type) != std::string::npos &&
                    strcmp(type, "struct") != 0 && strcmp(type, "union") != 0) {
                    replaceAll(raw_type, type, "");
                    attr_list.emplace_back(type);
                }
            }
            replaceAll(raw_type, " ", "");

            if (Lexical::SymbolUtils::IsPrimitiveType(raw_type.c_str())) {
                _named_types[name] =
                    std::make_shared<Type>(raw_type, attr_list);
                return _named_types[name];
            } else {
                return nullptr;
            }
        }
    }
}

bool ASTContext::addType(const std::string &name,
                         const std::shared_ptr<Type> &new_type) {
    // if we are adding global type
    if (_current_context == nullptr) {
        if (_named_types.find(name) != _named_types.end()) {
            DLOG(ERROR) << "Type " << name << " already exists";
            // TODO : print type table here
            return false;
        } else {
            DVLOG(AST_LOG_LEVEL)
                << "Add new Type [" << name << "] to global scope";
            // TODO : print type table here
            _named_types.insert(std::make_pair(name, new_type));
            return true;
        }
    } else {
        if (_current_context->hasType(name)) {
            DLOG(ERROR) << "Type " << name << " already exists";
            // TODO : print type table here
            return false;
        } else {
            DVLOG(AST_LOG_LEVEL)
                << "Add new Type [" << name << "] to local scope";
            // TODO : print type table here
            _current_context->addType(name, new_type);
            return true;
        }
    }
}

bool ASTContext::hasFunction(const std::string &name) {
    if (_function_table.find(name) != _function_table.end()) {
        return true;
    } else {
        return false;
    }
}

bool ASTContext::hasFunctionBody(const std::string &name) {
    if (_function_table.find(name + "_decl") != _function_table.end()) {
        return true;
    } else {
        return false;
    }
}

bool ASTContext::addFunction(
    const std::string &name, std::shared_ptr<Type> &output,
    const std::list<std::shared_ptr<Type>> &argument_list) {
    if (output == nullptr) {
        DLOG(FATAL) << "Output type is nullptr for function " << name;
    }

    if (_function_table.find(name) != _function_table.end()) {
        DLOG(ERROR) << "Function " << name << " already exists";
        return false;
    } else {
        DVLOG(AST_LOG_LEVEL) << "Add new Function [" << name << "]";
        _function_table.insert(
            std::make_pair(name, std::make_pair(output, argument_list)));
        return true;
    }
}

std::tuple<std::shared_ptr<Type>, std::list<std::shared_ptr<Type>>>
ASTContext::getFuncRetAndArgType(const std::basic_string<char> &name) {
    if (hasFunction(name)) {
        return _function_table[name];
    } else {
        DLOG(ERROR) << "Function " << name << " does not exist";
        return {nullptr, {}};
    }
}

bool ASTContext::addDecl(std::unique_ptr<ASTNode> node) {
    // node muse be not a nullptr
    if (node == nullptr) {
        return false;
    }

    // if we are adding a function Node
    if (node->IsDeclNode()) {
        std::unique_ptr<DeclNode> derivedPointer(
            dynamic_cast<DeclNode *>(node.release()));
        auto fuc_name = std::string(derivedPointer->GetName()) +
                        (derivedPointer->HasBody() ? "_decl" : "");

        // if we already have the
        auto node_ptr = std::find_if(
            _global_decl.begin(), _global_decl.end(),
            [&fuc_name](
                const std::pair<std::string, std::unique_ptr<DeclNode>> &func) {
                return func.first == fuc_name;
            });

        // we need to make sure function/decl name is unique
        if (node_ptr != _global_decl.end()) {
            DLOG(WARNING) << "Decl node with name: " << fuc_name
                          << " already exists, will be overwritten";
            node_ptr->second = std::move(derivedPointer);
        } else {
            // move ownership of decl node to global_decl
            _global_decl.emplace_back(fuc_name, std::move(derivedPointer));
        }
        return true;
    }

    DLOG(ERROR) << "Unsupported ASTNode type, only support decl node";
    return false;
}

void ASTContext::leaveScope() {
    if (_current_context == nullptr) {
        DLOG(FATAL) << "cannot leave scope, already at root";
        assert(false);
    } else {
        _current_context = _current_context->getUpperScope();
    }
}

void ASTContext::enterScope() {
    if (_current_context == nullptr) {
        DLOG(FATAL) << "Cannot enter scope while current context is null";
        assert(false);
    } else {
        auto new_context = _current_context->EnterScope();
        _current_context = new_context;
    }
}

void ASTContext::enterScope(const std::string &name,
                            const std::shared_ptr<Type> &return_type) {
    if (_current_context != nullptr) {
        DLOG(FATAL) << "Only root context can enter a named scope";
        assert(false);
    } else {
        _scoped_symbol_table[name] = std::make_shared<SymbolTable>(return_type);
        _current_context = _scoped_symbol_table[name];
    }
}

std::shared_ptr<Type> ASTContext::getType(
    std::shared_ptr<Type> name,
    const std::list<std::shared_ptr<AST::Type>> &argument_list) {
    return std::make_shared<FuncPtrType>(name, argument_list);
}

std::shared_ptr<Type> ASTContext::getType(
    const std::shared_ptr<AST::Type> &base_type,
    std::list<std::unique_ptr<AST::ASTNode>> &shape) {
    if (base_type == nullptr || !hasType(base_type->GetName())) {
        DLOG(ERROR) << "Type   not found";
        return nullptr;
    } else {
        std::shared_ptr<Type> current_type = getType(base_type->GetName());
        while (!shape.empty()) {
            auto shape_node = std::move(shape.front());
            shape.pop_front();
            if (shape_node == nullptr) {
                current_type = std::make_shared<ArrayType>(current_type);
            } else if (shape_node->IsDeducible()) {
                auto shape_value = shape_node->GetDeducedValue();
                if (shape_value.has_value()) {
                    current_type = std::make_shared<ArrayType>(
                        current_type, shape_value.value().AsInt());
                } else {
                    DLOG(FATAL)
                        << "Shape value is not available if node is deducible";
                }
            } else {
                current_type = std::make_shared<ArrayType>(
                    current_type, std::move(shape_node));
            }
        }

        return current_type;
    }
}

bool ASTContext::hasVariable(const std::string &name, bool current_scope) {
    if (!current_scope) {
        if (_current_context == nullptr ||
            !_current_context->hasVariable(name)) {
            for (auto &var : _global_decl) {
                if (var.first == name) {
                    if (var.second->IsFuncDecl()) {
                        return false;
                    }
                    return true;
                }
            }
            return false;
        } else {
            return _current_context->hasVariable(name);
        }
    } else {
        if (_current_context != nullptr) {
            return _current_context->hasVariable(name);
        } else {
            return false;
        }
    }
}

std::shared_ptr<Type> ASTContext::getVariableType(const std::string &name) {
    if (hasVariable(name)) {
        if (_current_context->hasVariable(name)) {
            return _current_context->getVariableType(name);
        } else {
            for (auto &var : _global_decl) {
                if (var.first == name) {
                    return var.second->GetType();
                }
            }
            DLOG(ERROR) << "Variable " << name << " not found";
            return nullptr;
        }
    } else {
        DLOG(ERROR) << "Variable " << name << " not found";
        return nullptr;
    }
}

Status ASTContext::addVariable(const std::string &name,
                               std::shared_ptr<Type> &token_types) {
    if (_current_context == nullptr) {
        DLOG(FATAL) << "should never call addVariable when in root context";
    } else {
        if (_current_context->hasVariable(name)) {
            DLOG(ERROR) << "Variable " << name << " already exists";
            return {Status::Code::INVALID_ARGUMENT,
                    "Variable " + name + " already exists"};
        } else {
            if (_current_context->addVariable(name, token_types)) {
                DVLOG(AST_LOG_LEVEL)
                    << "Add variable " << name << " to current context";
                return Status::OkStatus();
            } else {
                DLOG(ERROR) << "Failed to add variable " << name
                            << "when hasVariable is false";
                return {Status::Code::INTERNAL,
                        "failed to add variable " + name};
            }
        }
    }
    return Status::OkStatus();
}

std::string ASTContext::Dump() const {
    std::string ret;
#ifndef NDEBUG
    std::string indent;

    // Dump the function declarations
    indent = "|";
    ret += "Parsed AST: \n";
    for (auto &func : _global_decl) {
        ret += func.second->Dump(indent) + "\n";
    }

    return ret;
#else
    // Dump the function declarations
    for (auto &func : _global_decl) {
        if (func.second->IsFuncDecl()) {
            ret += "Line " + std::to_string(func.second->GetLine()) + ": " +
                   "Function " + func.second->GetType()->GetName() + " " +
                   func.first + "\n";

            ret += func.second->Dump("\t") + "\n";
        } else {
            ret += "Line " + std::to_string(func.second->GetLine()) + ": " +
                   "global " + func.second->GetType()->GetName() + " " +
                   func.first + "\n";
        }
    }
#endif
    return ret;
}
bool ASTContext::AtRot() { return _current_context == nullptr; }
}  // namespace Mycc::AST