//
// Created by chen_ on 2022/5/11.
//

#include "Ctx.h"

#include <memory>
#include <utility>

#include "ast/type/Type.h"
#include "parser/syntax/utils/string_utils.h"
namespace hzcc::syntax {

using AttrList = std::list<ast::Attribute>;

Ctx::Ctx(std::shared_ptr<ast::CompilationUnit> compilationUnit)
    : _compilationUnit(std::move(compilationUnit)) {
    _scoped_symbol_table["ROOT"] = std::make_shared<SymbTbl>(
        std::shared_ptr<ast::Type>(nullptr), std::shared_ptr<SymbTbl>(nullptr));
    _current_context = _scoped_symbol_table["ROOT"];
}

void Ctx::leave_scope() {
    if (_current_context.lock() == nullptr) {
        DLOG(FATAL) << "cannot leave scope, already at root";
    } else {
        _current_context = _current_context.lock()->getUpperScope();
    }
}

void Ctx::enter_scope() {
    if (_current_context.lock() == nullptr) {
        DLOG(FATAL) << "Cannot enter scope while current context is null";
    } else {
        auto new_context = _current_context.lock()->EnterScope();
        _current_context = new_context;
    }
}

void Ctx::enterScope(const std::string &name, const Ctx::TypePtr &return_type) {
    DLOG_IF(WARNING, return_type == nullptr)
        << "scope " << name << " has no return type";
    if (!isRoot()) {
        DLOG(FATAL) << "Only root context can enter a named scope";
    } else {
        _scoped_symbol_table[name] =
            std::make_shared<SymbTbl>(return_type, _current_context);
        _current_context = _scoped_symbol_table[name];
    }
}

/**
 * #############################################################
 * ###############    retType related Functions     ###############
 * #############################################################
 */

Ctx::TypePtr Ctx::SearchNamedType(std::string_view name) {
    if (name.empty()) {
        return nullptr;
    }

    // directly return true if the type is PrimitiveType
    if (parser_common::IsPrimitiveType(name.data())) {
        return ast::GetNumericalTypeOf(static_cast<ast::PrimitiveType>(
            parser_common::PrimitiveTypeID(name.data())));
    } else {
        // we fist check our global scope
        auto it = _named_types.find(name);
        if (it != _named_types.end()) {
            return it->second;
        } else {
            // then check the current scope
            if (_current_context.lock() != nullptr &&
                _current_context.lock()->hasType(name)) {
                return _current_context.lock()->getType(name);
            } else {
                return nullptr;
            }
        }
    }
}

bool Ctx::hasType(std::string_view basicString) {
    return SearchNamedType(basicString) != nullptr;
}

Ctx::TypePtr Ctx::get_type(const std::string &name,
                           const std::list<Token> &attr_list) {
    // directly return true if the type is PrimitiveType
    if (parser_common::IsPrimitiveType(name.data())) {
        if (attr_list.empty()) {
            return ast::GetNumericalTypeOf(static_cast<ast::PrimitiveType>(
                parser_common::PrimitiveTypeID(name.data())));
        } else {
            return std::make_shared<ast::NumericalType>(
                static_cast<ast::PrimitiveType>(
                    parser_common::PrimitiveTypeID(name.data())),
                attr_list);
        }
    } else {
        // we fist check our global scope
        auto it = _named_types.find(name);
        if (it != _named_types.end()) {
            return it->second;
        } else {
            // then check the current scope
            if (_current_context.lock() != nullptr &&
                _current_context.lock()->hasType(name)) {
                return _current_context.lock()->getType(name);
            } else {
                return nullptr;
            }
        }
    }
}

Ctx::TypePtr Ctx::getArrayType(const Ctx::TypePtr &base_type,
                               const TokenList &attr_list,
                               std::list<ast::ExprPtr> &shape) {
    // convert the shape to a list of TokenType
    AttrList attr_types;
    for (const auto &type : attr_list) {
        attr_types.emplace_back(type.Type());
    }

    if (base_type == nullptr || !hasType(base_type->Name())) {
        DLOG(ERROR) << "retType not found";
        return nullptr;
    } else {
        Ctx::TypePtr current_type = base_type;
        while (!shape.empty()) {
            auto shape_node = std::move(shape.front());
            shape.pop_front();
            current_type = std::make_shared<ast::ArrayType>(
                current_type, std::move(shape_node), attr_types);
        }

        return current_type;
    }
}

Ctx::TypePtr Ctx::getFuncPtrType(const std::string &name) { return nullptr; }

std::shared_ptr<ast::StructType> Ctx::addStructType(
    const std::string &struct_name, const std::list<Token> &attr_list) {
    auto name = "struct " + struct_name;

    // convert the shape to a list of TokenType
    std::list<TokenType> attr_types;
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
                << "Add new Struct retType [" << name << "] to global scope";
            auto new_type =
                std::make_shared<ast::StructType>(struct_name, attr_types);
            _named_types.insert(std::make_pair(name, new_type));

            // when leaving this function all attrs should be consumed
            DLOG_IF(FATAL, !attr_list.empty())
                << "When constructing new struct " << name
                << "its attributes should be consumed";

            return new_type;
        }
    } else {
        if (_current_context.lock()->hasType(name)) {
            LOG(FATAL) << "retType " << name << " already exists";
            return nullptr;
        } else {
            DVLOG(AST_LOG_LEVEL)
                << "Trying to add new retType [" << name << "] to local scope";

            // when leaving this function all attrs should be consumed
            DLOG_IF(FATAL, !attr_list.empty())
                << "When constructing new struct " << name
                << "its attributes should be consumed";

            return _current_context.lock()->addStructType(struct_name);
        }
    }
}

bool Ctx::hasVariable(const std::string &name, bool current_scope) {
    return SearchNamedType(name) != nullptr;
}

Ctx::TypePtr Ctx::getVariableType(const std::string &name) {
    if (hasVariable(name, false)) {
        return _current_context.lock()->getVariableType(name);
    } else {
        DLOG(ERROR) << "Variable " << name << " not found";
        return nullptr;
    }
}

std::pair<bool, int> Ctx::getVariableInfo(const std::basic_string<char> &name) {
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

void Ctx::addVariable(Position pos, const std::string &name,
                      Ctx::TypePtr &variable_type) {
    DLOG_ASSERT(_current_context.lock() != nullptr)
        << " should never call addVariable when in root context";
    DLOG_ASSERT(!_current_context.lock()->hasVariable(name, true))
        << "variable already exists";
    _current_context.lock()->addVariable(pos, name, variable_type);
}

bool Ctx::hasFunction(const std::string &name) {
    if (_function_def_table.find(name) != _function_def_table.end()) {
        return true;
    } else {
        return false;
    }
}

bool Ctx::hasFunctionBody(const std::string &name) {
    if (_function_def_table.find(name + "_decl") != _function_def_table.end()) {
        return true;
    } else {
        return false;
    }
}

bool Ctx::addFunction(Position line_no, const std::string &name,
                      Ctx::TypePtr &output,
                      const std::list<Ctx::TypePtr> &argument_list) {
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

std::tuple<TypePtr, std::list<TypePtr>, Position> Ctx::getFuncRetAndArgType(
    const std::basic_string<char> &name) {
    if (hasFunction(name)) {
        return {
            _function_def_table[name],
        };
    } else {
        DVLOG(SYNTAX_LOG_LEVEL) << "Function " << name << " does not exist";
        return {nullptr, {}, -1};
    }
}
void Ctx::addDecl(std::unique_ptr<ast::IDeclStmt> type) {
    if (type->IsDeclNode() && !type->IsFuncDecl()) {
        auto vartype = type->declType();
        _current_context.lock()->addVariable(type->Location().first,
                                             type->GetName(), vartype);
    }
    _compilationUnit->addDecl(std::move(type));
}

}  // namespace hzcc::syntax
