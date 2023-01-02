//
// Created by chen_ on 2022/4/9.
//
#include "SymbTbl.h"

#include <glog/logging.h>
#include <memory>
#include <utility>
#include <ostream>

#include "ast/type/Type.h"
#include "utils/logging.h"

namespace hzcc::ast {
SymbTbl::SymbTbl(std::shared_ptr<ast::Type> return_type,
                 std::weak_ptr<SymbTbl> parent)
    : _upper_scope_table(std::move(parent)),
      _return_type(std::move(return_type)) {}

std::shared_ptr<SymbTbl> SymbTbl::getUpperScope() {
    return _upper_scope_table.lock();
}

// NOLINTNEXTLINE(misc-no-recursion)
bool SymbTbl::hasType(std::string_view name) {
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
std::shared_ptr<ast::StructType> SymbTbl::addStructType(
    const std::string& name) {
    if (hasType("struct " + name)) {
        LOG(FATAL) << "retType " << name << " has already been defined";
    } else {
        auto new_type = std::make_shared<ast::StructType>(name);

        _named_types.insert(std::make_pair("struct " + name, new_type));
        return new_type;
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
std::shared_ptr<ast::Type> SymbTbl::getType(std::string_view name) {
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

// NOLINTNEXTLINE(misc-no-recursion)
bool SymbTbl::hasVariable(std::string_view name, bool current_scope) {
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

void SymbTbl::addVariable(Position pos,                               // NOLINT
                          std::string_view name,                      // NOLINT
                          std::shared_ptr<ast::Type>& token_types) {  // NOLINT
    DLOG_ASSERT(!hasType(name) || name.find("struct ") != std::string::npos)
        << " Variable " << name << " is already defined as a type";
    DLOG_ASSERT(token_types != nullptr)
        << " Variable " << name << " has passed a nullptr type";
    DLOG_ASSERT(!hasVariable(name, true))
        << " Variable " << name << " has already been defined";
    DVLOG(SYNTAX_LOG_LEVEL) << "Adding variable " << name;
    _variable_lookup_table.insert(
        std::make_pair(name, std::make_pair(pos, token_types)));
}

// NOLINTNEXTLINE(misc-no-recursion)
std::shared_ptr<ast::Type> SymbTbl::getVariableType(const std::string& name) {
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
std::shared_ptr<SymbTbl> SymbTbl::EnterScope() {
    auto current_scope = shared_from_this();
    auto new_scope = std::make_shared<SymbTbl>(_return_type, current_scope);
    _scoped_contexts.push_back(new_scope);
    return new_scope;
}

// NOLINTNEXTLINE(misc-no-recursion)
int SymbTbl::getVariableDeclLine(const std::string& name) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, hasVariable(name, false))
        << "Variable " << name << " has not been defined";
#endif

    if (_variable_lookup_table.find(name) != _variable_lookup_table.end()) {
        return _variable_lookup_table[name].first.second;
    } else {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->getVariableDeclLine(name);
        } else {
            LOG(FATAL) << "Variable " << name << " has not been defined";
        }
    }
}
std::shared_ptr<ast::Type> SymbTbl::GetReturnType() { return _return_type; }

}  // namespace hzcc::ast
