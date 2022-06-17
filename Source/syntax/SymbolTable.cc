//
// Created by chen_ on 2022/4/9.
//
#include "SymbolTable.h"

#include <cassert>
#include <memory>

#include "AST/type/StructType.h"
#include "AST/type/Type.h"
#include "lexical/Token.h"
#include "utils/logging.h"
namespace Hzcc::Syntax {
SymbolTable::SymbolTable(std::shared_ptr<AST::Type> return_type,
                         std::weak_ptr<SymbolTable> parent)
    : _upper_scope_table(parent), _return_type(return_type) {}

std::shared_ptr<SymbolTable> SymbolTable::getUpperScope() {
    return _upper_scope_table.lock();
}
bool SymbolTable::hasType(const std::string& name) {
    if (_named_types.find(name) != _named_types.end()) {
        return true;
    } else {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->hasType(name);
        } else {
            return false;
        }
    }
}
std::shared_ptr<AST::StructType> SymbolTable::addStructType(
    const std::string& name) {
    if (hasType("struct " + name)) {
        LOG(FATAL) << "RetType " << name << " has already been defined";
    } else {
        auto new_type = std::make_shared<AST::StructType>(
            name);

        _named_types.insert(std::make_pair("struct " + name, new_type));
        return new_type;
    }
}

std::shared_ptr<AST::Type> SymbolTable::getType(const std::string& name) {
    if (!hasType(name)) return nullptr;
    if (_named_types.find(name) != _named_types.end()) {
        return _named_types[name];
    } else {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->getType(name);
        } else {
            return nullptr;
        }
    }
}
bool SymbolTable::hasVariable(const std::string& name, bool current_scope) {
    if (_variable_lookup_table.find(name) != _variable_lookup_table.end()) {
        return true;
    } else if (!current_scope) {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->hasVariable(name, false);
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void SymbolTable::addVariable(uint64_t line_no, const std::string& name,
                              std::shared_ptr<AST::Type>& token_types) {
    DLOG_ASSERT(!hasType(name) || name.find("struct ") != std::string::npos)
        << " Variable " << name << " is already defined as a type";
    DLOG_ASSERT(token_types != nullptr)
        << " Variable " << name << " has passed a nullptr type";
    DLOG_ASSERT(!hasVariable(name, true))
        << " Variable " << name << " has already been defined";
    DVLOG(SYNTAX_LOG_LEVEL) << "Adding variable " << name;
    _variable_lookup_table.insert(
        std::make_pair(name, std::make_pair(line_no, token_types)));
}

std::shared_ptr<AST::Type> SymbolTable::getVariableType(
    const std::string& name) {
    if (_variable_lookup_table.find(name) != _variable_lookup_table.end()) {
        return _variable_lookup_table[name].second;
    } else {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->getVariableType(name);
        } else {
            LOG(FATAL) << "Variable " << name << " has not been defined";
        }
    }
}
std::shared_ptr<SymbolTable> SymbolTable::EnterScope() {
    auto current_scope = shared_from_this();
    auto new_scope = std::make_shared<SymbolTable>(_return_type, current_scope);
    _scoped_contexts.push_back(new_scope);
    return new_scope;
}

int SymbolTable::getVariableDeclLine(const std::string& name) {
    DLOG_ASSERT(hasVariable(name, false))
        << "Variable " << name << " has not been defined";

    if (_variable_lookup_table.find(name) != _variable_lookup_table.end()) {
        return _variable_lookup_table[name].first;
    } else {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->getVariableDeclLine(name);
        } else {
            LOG(FATAL) << "Variable " << name << " has not been defined";
        }
    }
}
std::shared_ptr<AST::Type> SymbolTable::GetReturnType() { return _return_type; }

}  // namespace Hzcc::Syntax
