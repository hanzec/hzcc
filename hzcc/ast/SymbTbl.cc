//
// Created by chen_ on 2022/4/9.
//
#include "SymbTbl.h"

#include <glog/logging.h>

#include <memory>
#include <ostream>
#include <utility>

#include "ast/type/Type.h"
#include "utils/logging.h"

namespace hzcc::ast {
SymbTbl::SymbTbl(QualTypePtr return_type, std::weak_ptr<SymbTbl> parent)
    : _upper_scope_table(std::move(parent)),
      _return_type(std::move(return_type)) {}

std::shared_ptr<SymbTbl> SymbTbl::getUpperScope() {
    return _upper_scope_table.lock();
}

// NOLINTNEXTLINE(misc-no-recursion)
bool SymbTbl::has_type(std::string_view name) {
    if (_named_types.find(name) != _named_types.end()) {
        return true;
    } else {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->has_type(name);
        } else {
            return false;
        }
    }
}
StructTypePtr SymbTbl::add_struct_type(std::string_view name) {
    if (has_type("struct " + std::string(name))) {
        LOG(FATAL) << "type " << name << " has already been defined";
    } else {
        auto new_type = std::make_shared<ast::StructType>(name);

        _named_types.insert(
            std::make_pair("struct " + std::string(name), new_type));
        return new_type;
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
QualTypePtr SymbTbl::get_type(std::string_view name,
                              const std::list<Qualifier>& qualifiers) {
    if (!has_type(name)) return nullptr;
    if (_named_types.find(name) != _named_types.end()) {
        return std::make_shared<QualType>(_named_types[name], qualifiers);
    } else {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->get_type(name);
        } else {
            LOG(ERROR) << "Variable " << name << " has not been defined";
            return nullptr;
        }
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
bool SymbTbl::has_var(std::string_view name, bool current_scope) {
    if (_variable_lookup_table.find(name) != _variable_lookup_table.end()) {
        return true;
    } else if (!current_scope) {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->has_var(name, false);
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void SymbTbl::add_var(Position pos,             // NOLINT
                      QualTypePtr& type,        // NOLINT
                      std::string_view name) {  // NOLINT
    DLOG_ASSERT(type != nullptr)
        << " Variable " << name << " has passed a nullptr type";
    DLOG_ASSERT(!has_type(name) || name.find("struct ") != std::string::npos)
        << " Variable " << name << " is already defined as a type";
    DLOG_ASSERT(!has_var(name, true))
        << " Variable " << name << " has already been defined";
    DVLOG(SYNTAX_LOG) << "Adding variable " << name;
    _variable_lookup_table.emplace(
        std::make_pair(name, std::make_pair(pos, type)));
}

std::shared_ptr<SymbTbl> SymbTbl::enter_scope() {
    auto current_scope = shared_from_this();
    auto new_scope = std::make_shared<SymbTbl>(_return_type, current_scope);
    _scoped_contexts.push_back(new_scope);
    return new_scope;
}

// NOLINTNEXTLINE(misc-no-recursion)
Position SymbTbl::var_def_pos(std::string_view name) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, has_var(name, false))
        << "Variable " << name << " has not been defined";
#endif

    if (_variable_lookup_table.find(name) != _variable_lookup_table.end()) {
        return _variable_lookup_table[name].first;
    } else {
        if (_upper_scope_table.lock() != nullptr) {
            return _upper_scope_table.lock()->var_def_pos(name);
        } else {
            LOG(FATAL) << "Variable " << name << " has not been defined";
        }
    }
}

}  // namespace hzcc::ast
