//
// Created by chen_ on 2022/4/9.
//
#include "SymbolTable.h"

#include <cassert>

#include "AST/type/StructType.h"
#include "AST/type/Type.h"
#include "utils/logging.h"

namespace Mycc::AST {
SymbolTable::SymbolTable(std::shared_ptr<Type> return_type,
                         std::shared_ptr<SymbolTable> parent)
    : _upper_scope_table(std::move(parent)),
      _return_type(std::move(return_type)) {}

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
std::shared_ptr<StructType> SymbolTable::addStructType(
    const std::string& name) {
    if (hasType("struct " + name)) {
        LOG(FATAL) << "Type " << name << " has already been defined";
    } else {
        auto new_type = std::shared_ptr<StructType>(new StructType(name, {}));

        _named_types.insert(std::make_pair("struct " + name, new_type));
        return new_type;
    }
}

std::shared_ptr<Type> SymbolTable::getType(const std::string& name) {
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
bool SymbolTable::hasVariable(const std::string& name) {
    if (_variable_lookup_table.find(name) != _variable_lookup_table.end()) {
        return true;
    } else {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->hasVariable(name);
        } else {
            return false;
        }
    }
}

bool SymbolTable::addVariable(const std::string& name,
                              std::shared_ptr<Type>& token_types) {
    if (hasVariable(name) || token_types == nullptr || hasType(name)) {
        return false;
    } else {
        _variable_lookup_table.insert(std::make_pair(name, token_types));
        return true;
    }
}
std::shared_ptr<Type> SymbolTable::getVariableType(const std::string& name) {
    if (_variable_lookup_table.find(name) != _variable_lookup_table.end()) {
        return _variable_lookup_table[name];
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

}  // namespace Mycc::AST
