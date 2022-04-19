#include "CompilationUnit.h"

#include <cassert>
#include <iomanip>
#include <utility>

#include "AST/DeduceValue.h"
#include "AST/statement/compound.h"
#include "AST/type/ArrayType.h"
#include "AST/type/FuncPtrType.h"
#include "AST/type/StructType.h"
#include "AST/utils/debug_utils.h"
#include "lexical/utils/symbol_utils.h"
#include "options.h"
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

bool CompilationUnit::hasType(const std::string &basicString) {
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
            if (_current_context.lock() != nullptr &&
                _current_context.lock()->hasType(basicString)) {
                return true;
            } else {
                return false;
            }
        }
    };
}

std::shared_ptr<Type> CompilationUnit::getNamedType(
    const std::string &name, const std::list<Lexical::TokenType> &attr_list) {
    auto final_name = name;
    for (const auto &type : attr_list) {
        final_name.insert(0,
                          Lexical::SymbolUtils::TokenTypeToString(type) + " ");
    }

    if (_current_context.lock() != nullptr &&
        _current_context.lock()->hasType(final_name)) {
        return _current_context.lock()->getType(final_name);
    } else {
        if (_named_types.find(name) != _named_types.end()) {
            if (attr_list.empty()) {
                return _named_types[name];
            } else {
                // TODO: modify here since assume only accept const attribute
                return Type::GetBasicType(_named_types[name]->GetName(),
                                          {Lexical::TokenType::kConst});
            }
        } else {
            if (Lexical::SymbolUtils::IsPrimitiveType(name.c_str())) {
                return Type::GetBasicType(name, attr_list);
            } else {
                return nullptr;
            }
        }
    }
}

std::shared_ptr<StructType> CompilationUnit::addStructType(
    const std::string &struct_name, std::list<Lexical::TokenType> &attr_list) {
    auto name = "struct " + struct_name;

    // if we are adding global type
    if (_current_context.lock() == nullptr) {
        if (_named_types.find(name) != _named_types.end()) {
            LOG(FATAL) << "Struct " << name << " already exists";
            return nullptr;
        } else {
            DVLOG(AST_LOG_LEVEL)
                << "Add new Struct Type [" << name << "] to global scope";
            auto new_type =
                std::shared_ptr<StructType>(new StructType(struct_name, {}));
            _named_types.insert(std::make_pair(name, new_type));

            // when leaving this function all attrs should be consumed
            DLOG_IF(FATAL, !attr_list.empty())
                << "When constructing new struct " << name
                << "its attributes should be consumed";

            return new_type;
        }
    } else {
        if (_current_context.lock()->hasType(name)) {
            LOG(FATAL) << "Type " << name << " already exists";
            return nullptr;
        } else {
            DVLOG(AST_LOG_LEVEL)
                << "Trying to add new Type [" << name << "] to local scope";

            // when leaving this function all attrs should be consumed
            DLOG_IF(FATAL, !attr_list.empty())
                << "When constructing new struct " << name
                << "its attributes should be consumed";

            return _current_context.lock()->addStructType(struct_name);
        }
    }
}

bool CompilationUnit::hasFunction(const std::string &name) {
    if (_function_table.find(name) != _function_table.end()) {
        return true;
    } else {
        return false;
    }
}

bool CompilationUnit::hasFunctionBody(const std::string &name) {
    if (_function_table.find(name + "_decl") != _function_table.end()) {
        return true;
    } else {
        return false;
    }
}

bool CompilationUnit::addFunction(
    int line_no, const std::string &name, std::shared_ptr<Type> &output,
    const std::list<std::shared_ptr<Type>> &argument_list) {
    if (output == nullptr) {
        DLOG(FATAL) << "Output type is nullptr for function " << name;
    }

    if (_function_table.find(name) != _function_table.end()) {
        DLOG(ERROR) << "Function " << name << " already exists";
        return false;
    } else {
        DVLOG(AST_LOG_LEVEL) << "Add new Function [" << name << "]";
        _function_table.insert(std::make_pair(
            name, std::make_tuple(output, argument_list, line_no)));
        return true;
    }
}

std::tuple<std::shared_ptr<Type>, std::list<std::shared_ptr<Type>>, int>
CompilationUnit::getFuncRetAndArgType(const std::basic_string<char> &name) {
    if (hasFunction(name)) {
        return {
            _function_table[name],
        };
    } else {
        DVLOG(SYNTAX_LOG_LEVEL) << "Function " << name << " does not exist";
        return {nullptr, {}, -1};
    }
}

void CompilationUnit::addDecl(std::unique_ptr<ASTNode> node) {
    // node muse be not a nullptr
    DLOG_ASSERT(node != nullptr) << " Node is nullptr";

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
        if (node_ptr != _global_decl.end() &&
            Options::Global_enable_naming_checking) {
            DLOG(FATAL) << "Decl name: " << fuc_name << " already defined !";
        } else {
            // move ownership of decl node to global_decl
            _global_decl.emplace_back(fuc_name, std::move(derivedPointer));
        }
    } else {
        DLOG(FATAL) << "Unsupported ASTNode type, only support decl node";
    }
}

void CompilationUnit::leaveScope() {
    if (_current_context.lock() == nullptr) {
        DLOG(FATAL) << "cannot leave scope, already at root";
    } else {
        _current_context = _current_context.lock()->getUpperScope();
    }
}

void CompilationUnit::enterScope() {
    if (_current_context.lock() == nullptr) {
        DLOG(FATAL) << "Cannot enter scope while current context is null";
    } else {
        auto new_context = _current_context.lock()->EnterScope();
        _current_context = new_context;
    }
}

void CompilationUnit::enterScope(const std::string &name,
                            const std::shared_ptr<Type> &return_type) {
    if (_current_context.lock() != nullptr) {
        DLOG(FATAL) << "Only root context can enter a named scope";
    } else {
        _scoped_symbol_table[name] =
            std::make_shared<SymbolTable>(return_type, nullptr);
        _current_context = _scoped_symbol_table[name];
    }
}

std::shared_ptr<Type> CompilationUnit::getFuncPtrType(
    std::shared_ptr<Type> name,
    const std::list<std::shared_ptr<AST::Type>> &attrs) {
    return std::make_shared<FuncPtrType>(name, attrs);
}

std::shared_ptr<Type> CompilationUnit::getArrayType(
    const std::shared_ptr<AST::Type> &base_type,
    std::list<std::unique_ptr<AST::ASTNode>> &shape) {
    if (base_type == nullptr || !hasType(base_type->GetName())) {
        DLOG(ERROR) << "Type not found";
        return nullptr;
    } else {
        std::shared_ptr<Type> current_type = base_type;
        while (!shape.empty()) {
            auto shape_node = std::move(shape.front());
            shape.pop_front();
            current_type = ArrayType::GetArrayOfBasicType(
                current_type, std::move(shape_node), {});
        }

        return current_type;
    }
}

bool CompilationUnit::hasVariable(const std::string &name, bool current_scope) {
    if (!current_scope) {
        if (_current_context.lock() == nullptr ||
            !_current_context.lock()->hasVariable(name, false)) {
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
            return _current_context.lock()->hasVariable(name, false);
        }
    } else {
        if (_current_context.lock() != nullptr) {
            return _current_context.lock()->hasVariable(name, true);
        } else {
            for (auto &var : _global_decl) {
                if (var.first == name) {
                    if (var.second->IsFuncDecl()) {
                        return false;
                    }
                    return true;
                }
            }
            return false;
        }
    }
}

std::shared_ptr<Type> CompilationUnit::getVariableType(const std::string &name) {
    if (hasVariable(name, false)) {
        if (_current_context.lock()->hasVariable(name, false)) {
            return _current_context.lock()->getVariableType(name);
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

std::pair<bool, int> CompilationUnit::getVariableInfo(
    const std::basic_string<char> &name) {
    DLOG_ASSERT(name.empty() == false) << "Variable name cannot be empty";
    DLOG_ASSERT(hasVariable(name, false))
        << "Variable " << name << " not found";

    if (_current_context.lock() != nullptr &&
        _current_context.lock()->hasVariable(name, false)) {
        return std::make_pair(
            false, _current_context.lock()->getVariableDeclLine(name));
    } else {
        for (auto &var : _global_decl) {
            if (var.first == name) {
                return std::make_pair(true, var.second->GetLine());
            }
        }
        DLOG(ERROR) << "Variable " << name << " not found";
        return std::make_pair(false, 0);
    }
}

void CompilationUnit::addVariable(int line_no, const std::string &name,
                             std::shared_ptr<Type> &variable_type) {
    DLOG_ASSERT(_current_context.lock() != nullptr)
        << " should never call addVariable when in root context";
    DLOG_ASSERT(!_current_context.lock()->hasVariable(name, true))
        << "variable already exists";
    _current_context.lock()->addVariable(line_no, name, variable_type);
}

std::string CompilationUnit::Dump() const {
    std::stringstream ret;
#ifndef NDEBUG
    std::string indent;

    // Dump the function declarations
    indent = "|";
    ret << "Parsed AST: \n";
    for (auto &func : _global_decl) {
        ret << func.second->Dump(indent) +
                   (func == _global_decl.back() ? "" : "\n");
    }

    return ret.str();
#else
    // Dump the function declarations
    for (auto &func : _global_decl) {
        if (func.second->IsFuncDecl()) {
            if (func.second->HasBody()) {
                ret << "Line " << std::setw(3)
                    << std::to_string(func.second->GetLine() + 1)
                    << std::setw(0)
                    << ": function " + func.second->GetType()->GetName() + " " +
                           func.second->GetName() + "\n";
                ret << func.second->Dump("\t");
            }
        } else {
            auto type_name = func.second->GetType()->GetName();
            if (type_name.find('[') != std::string::npos) {
                ret << "Line " << std::setw(3)
                    << std::to_string(func.second->GetLine() + 1)
                    << std::setw(0)
                    << ": global " +
                           func.second->GetType()->GetName().substr(
                               0, type_name.find_first_of('[')) +
                           " " + func.first +
                           func.second->GetType()->GetName().substr(
                               type_name.find_first_of('['),
                               type_name.find_last_of(']')) +
                           "\n";
            } else {
                ret << "Line " << std::setw(3)
                    << std::to_string(func.second->GetLine() + 1)
                    << std::setw(0)
                    << ": global " + func.second->GetType()->GetName() + " " +
                           func.first + "\n";
            }
        }
    }
#endif
    return ret.str();
}
bool CompilationUnit::AtRoot() { return _current_context.lock() == nullptr; }
CompilationUnit::~CompilationUnit() {}
CompilationUnit::CompilationUnit(std::string file_name)
    : _file_name(std::move(file_name)) {}
std::string CompilationUnit::GetFileName() const { return _file_name; }
std::shared_ptr<Type> CompilationUnit::GetReturnType() {
    DLOG_ASSERT(_current_context.lock() != nullptr)
        << " should never call GetReturnType when in root context";
    return _current_context.lock()->GetReturnType();
}

}  // namespace Mycc::AST
