//
// Created by chen_ on 2022/5/11.
//

#include "SyntaxContext.h"

#include <memory>
#include <utility>

#include "AST/type/ArrayType.h"
#include "AST/type/FuncPtrType.h"
#include "AST/type/StructType.h"
#include "lexical/utils/symbol_utils.h"
#include "syntax/utils/string_utils.h"
namespace Hzcc::Syntax {
SyntaxContext::SyntaxContext(
    std::shared_ptr<AST::CompilationUnit> compilationUnit)
    : _compilationUnit(std::move(compilationUnit)) {
    _scoped_symbol_table["ROOT"] =
        std::make_shared<SymbolTable>(std::shared_ptr<AST::Type>(nullptr),
                                      std::shared_ptr<SymbolTable>(nullptr));
    _current_context = _scoped_symbol_table["ROOT"];
}

void SyntaxContext::leaveScope() {
    if (_current_context.lock() == nullptr) {
        DLOG(FATAL) << "cannot leave scope, already at root";
    } else {
        _current_context = _current_context.lock()->getUpperScope();
    }
}

void SyntaxContext::enterScope() {
    if (_current_context.lock() == nullptr) {
        DLOG(FATAL) << "Cannot enter scope while current context is null";
    } else {
        auto new_context = _current_context.lock()->EnterScope();
        _current_context = new_context;
    }
}

void SyntaxContext::enterScope(const std::string &name,
                               const SyntaxContext::TypePtr &return_type) {
    DLOG_IF(WARNING, return_type == nullptr)
        << "scope " << name << " has no return type";
    if (!AtRoot()) {
        DLOG(FATAL) << "Only root context can enter a named scope";
    } else {
        _scoped_symbol_table[name] =
            std::make_shared<SymbolTable>(return_type, _current_context);
        _current_context = _scoped_symbol_table[name];
    }
}

/**
 * #############################################################
 * ###############    Type related Functions     ###############
 * #############################################################
 */

bool SyntaxContext::hasType(const std::string &basicString) {
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

SyntaxContext::TypePtr SyntaxContext::getNamedType(
    const std::string &name, const std::list<Lexical::Token> &attr_list) {
    std::list<Lexical::TokenType> attr_types;
    for (const auto &type : attr_list) {
        attr_types.emplace_back(type.Type());
    }

    if (_current_context.lock() != nullptr &&
        _current_context.lock()->hasType(name)) {
        return _current_context.lock()->getType(name);
    } else {
        if (_named_types.find(name) != _named_types.end()) {
            if (attr_list.empty()) {
                return _named_types[name];
            } else {
                return AST::Type::GetTypeOf(_named_types[name], attr_types);
            }
        } else {
            if (Lexical::SymbolUtils::IsPrimitiveType(name.c_str())) {
                return AST::Type::GetTypeOf(name, attr_types);
            } else {
                return nullptr;
            }
        }
    }
}

SyntaxContext::TypePtr SyntaxContext::getArrayType(
    const SyntaxContext::TypePtr &base_type,
    const std::list<Lexical::Token> &attr_list,
    std::list<std::unique_ptr<AST::ASTNode>> &shape) {
    // convert the shape to a list of TokenType
    std::list<Lexical::TokenType> attr_types;
    for (const auto &type : attr_list) {
        attr_types.emplace_back(type.Type());
    }

    if (base_type == nullptr || !hasType(base_type->GetName())) {
        DLOG(ERROR) << "Type not found";
        return nullptr;
    } else {
        SyntaxContext::TypePtr current_type = base_type;
        while (!shape.empty()) {
            auto shape_node = std::move(shape.front());
            shape.pop_front();
            current_type = AST::ArrayType::GetArrayOfBasicType(
                current_type, std::move(shape_node), attr_types);
        }

        return current_type;
    }
}

SyntaxContext::TypePtr SyntaxContext::getFuncPtrType(const std::string &name) {
    return AST::FuncPtrType::GetTypeOf(name, std::list<Lexical::TokenType>());
}

std::shared_ptr<AST::StructType> SyntaxContext::addStructType(
    const std::string &struct_name, std::list<Lexical::Token> &attr_list) {
    auto name = "struct " + struct_name;

    // convert the shape to a list of TokenType
    std::list<Lexical::TokenType> attr_types;
    for (const auto &type : attr_list) {
        attr_types.emplace_back(type.Type());
    }

    // if we are adding global type
    if (_current_context.lock() == nullptr) {
        if (_named_types.find(name) != _named_types.end()) {
            LOG(FATAL) << "Struct " << name << " already exists";
            return nullptr;
        } else {
            DVLOG(AST_LOG_LEVEL)
                << "Add new Struct Type [" << name << "] to global scope";
            auto new_type =
                std::make_shared<AST::StructType>(struct_name, attr_types);
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

bool SyntaxContext::hasVariable(const std::string &name, bool current_scope) {
    return _current_context.lock()->hasVariable(name, current_scope);
}

SyntaxContext::TypePtr SyntaxContext::getVariableType(const std::string &name) {
    if (hasVariable(name, false)) {
        return _current_context.lock()->getVariableType(name);
    } else {
        DLOG(ERROR) << "Variable " << name << " not found";
        return nullptr;
    }
}

std::pair<bool, int> SyntaxContext::getVariableInfo(
    const std::basic_string<char> &name) {
    DLOG_ASSERT(name.empty() == false) << "Variable name cannot be empty";
    DLOG_ASSERT(hasVariable(name, false))
        << "Variable " << name << " not found";

    if (_current_context.lock() != nullptr &&
        _current_context.lock()->hasVariable(name, false)) {
        return std::make_pair(
            false, _current_context.lock()->getVariableDeclLine(name));
    } else {
        DLOG(ERROR) << "Variable " << name << " not found";
        return std::make_pair(false, 0);
    }
}

void SyntaxContext::addVariable(int line_no, const std::string &name,
                                SyntaxContext::TypePtr &variable_type) {
    DLOG_ASSERT(_current_context.lock() != nullptr)
        << " should never call addVariable when in root context";
    DLOG_ASSERT(!_current_context.lock()->hasVariable(name, true))
        << "variable already exists";
    _current_context.lock()->addVariable(line_no, name, variable_type);
}

bool SyntaxContext::hasFunction(const std::string &name) {
    if (_function_def_table.find(name) != _function_def_table.end()) {
        return true;
    } else {
        return false;
    }
}

bool SyntaxContext::hasFunctionBody(const std::string &name) {
    if (_function_def_table.find(name + "_decl") != _function_def_table.end()) {
        return true;
    } else {
        return false;
    }
}

bool SyntaxContext::addFunction(
    int line_no, const std::string &name, SyntaxContext::TypePtr &output,
    const std::list<SyntaxContext::TypePtr> &argument_list) {
    if (output == nullptr) {
        DLOG(FATAL) << "Output type is nullptr for function " << name;
    }

    if (_function_def_table.find(name) != _function_def_table.end()) {
        DLOG(ERROR) << "Function " << name << " already exists";
        return false;
    } else {
        DVLOG(AST_LOG_LEVEL) << "Add new Function [" << name << "]";
        _function_def_table.insert(std::make_pair(
            name, std::make_tuple(output, argument_list, line_no)));
        return true;
    }
}

std::tuple<SyntaxContext::TypePtr, std::list<SyntaxContext::TypePtr>, int>
SyntaxContext::getFuncRetAndArgType(const std::basic_string<char> &name) {
    if (hasFunction(name)) {
        return {
            _function_def_table[name],
        };
    } else {
        DVLOG(SYNTAX_LOG_LEVEL) << "Function " << name << " does not exist";
        return {nullptr, {}, -1};
    }
}
void SyntaxContext::addDecl(std::unique_ptr<AST::DeclNode> type) {
    if (type->IsDeclNode() && !type->IsFuncDecl()) {
        auto vartype = type->GetType();
        _current_context.lock()->addVariable(type->GetLine(), type->GetName(),
                                             vartype);
    }
    _compilationUnit->addDecl(std::move(type));
}
}  // namespace Hzcc::Syntax