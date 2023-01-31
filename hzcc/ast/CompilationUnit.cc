#include "CompilationUnit.h"

#include <glog/logging.h>

#include <algorithm>
#include <ostream>

#include "ast/Stmt.h"
#include "ast/type/Type.h"
#include "parser/common/name_utils.h"

namespace hzcc::ast {
CompilationUnit::CompilationUnit(std::string file_name)
    : _file_name(std::move(file_name)) {
    _scoped_symbol_table["ROOT"] = std::make_shared<SymbTbl>(
        std::shared_ptr<ast::Type>(nullptr), std::shared_ptr<SymbTbl>(nullptr));
    _current_context = _scoped_symbol_table["ROOT"];
}

/**
 * ################################################################
 * #################   Scope related Functions  ###################
 * ################################################################
 */

void CompilationUnit::leave_scope() {
    if (_current_context.lock() == nullptr) {
        DLOG(FATAL) << "cannot leave scope, already at root";
    } else {
        _current_context = _current_context.lock()->getUpperScope();
    }
}

bool CompilationUnit::at_root() const {
    return _current_context.lock()->ret_type() == nullptr;
}

void CompilationUnit::enter_scope(std::string_view name) {
    if (name.empty()) {
        INTERNAL_LOG_IF(FATAL, _scoped_symbol_table.find(name) ==
                                   _scoped_symbol_table.end())
            << "cannot enter scope " << name << ", not found";
        _current_context = _scoped_symbol_table[name];
    } else {
        INTERNAL_LOG_IF(FATAL, at_root())
            << "cannot enter unnamed scope, if already at root";

        auto new_context = _current_context.lock()->enter_scope();
        _current_context = new_context;
    }
}

void CompilationUnit::create_func(std::string_view name,
                                  const TypePtr &return_type) {
    DLOG_IF(WARNING, return_type == nullptr)
        << "scope " << name << " has no return type";
    if (!at_root()) {
        DLOG(FATAL) << "Only root context can enter a named scope";
    } else {
        _scoped_symbol_table[name] =
            std::make_shared<SymbTbl>(return_type, _current_context);
        _current_context = _scoped_symbol_table[name];
    }
}

std::optional<ast::TypePtr> CompilationUnit::ret_type() const {
    if (at_root()) {
        return std::nullopt;
    } else {
        return _current_context.lock()->ret_type();
    }
}

/**
 * ################################################################
 * #################   Type related Functions  ####################
 * ################################################################
 */

bool CompilationUnit::has_type(std::string_view basicString) {
    return search_type(basicString) != nullptr;
}

TypePtr CompilationUnit::get_type(std::string_view name,
                                  std::list<Attribute> attr_list) {
    // directly return true if the type is PrimitiveType
    if (parser_common::IsPrimitiveType(name.data())) {
        if (attr_list.empty()) {
            return ast::GetNumericalTypeOf(
                parser_common::GetPrimitiveType(name));
        } else {
            return std::make_shared<ast::NumericalType>(
                parser_common::GetPrimitiveType(name), attr_list);
        }
    } else {
        // we fist check our global scope
        auto it = _named_types.find(name);
        if (it != _named_types.end()) {
            return it->second;
        } else {
            // then check the current scope
            if (_current_context.lock() != nullptr &&
                _current_context.lock()->has_type(name)) {
                return _current_context.lock()->get_type(name);
            } else {
                return nullptr;
            }
        }
    }
}

TypePtr CompilationUnit::search_type(std::string_view name) {
    if (name.empty()) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG(FATAL) << "Cannot search for empty type name";
#else
        return nullptr;
#endif
    }

    // directly return true if the type is PrimitiveType
    if (parser_common::IsPrimitiveType(name)) {
        return ast::GetNumericalTypeOf(parser_common::GetPrimitiveType(name));
    } else {
        // we fist check our global scope
        auto it = _named_types.find(name);
        if (it != _named_types.end()) {
            return it->second;
        } else {
            // then check the current scope
            if (_current_context.lock() != nullptr &&
                _current_context.lock()->has_type(name)) {
                return _current_context.lock()->get_type(name);
            } else {
                return nullptr;
            }
        }
    }
}

StructTypePtr CompilationUnit::add_struct_type(std::string_view struct_name,
                                               std::list<Attribute> attr_list) {
    auto name = "struct " + std::string(struct_name);

    // if we are adding global type
    if (_current_context.lock() == nullptr) {
        if (_named_types.find(name) != _named_types.end()) {
            LOG(FATAL) << "Struct " << name << " already exists";
            return nullptr;
        } else {
            DVLOG(AST_LOG_LEVEL)
                << "Add new Struct type [" << name << "] to global scope";
            auto new_type =
                std::make_shared<ast::StructType>(struct_name, attr_list);
            _named_types.insert(std::make_pair(name, new_type));

            // when leaving this function all attrs should be consumed
            DLOG_IF(FATAL, !attr_list.empty())
                << "When constructing new struct " << name
                << "its attributes should be consumed";

            return new_type;
        }
    } else {
        if (_current_context.lock()->has_type(name)) {
            LOG(FATAL) << "type " << name << " already exists";
            return nullptr;
        } else {
            DVLOG(AST_LOG_LEVEL)
                << "Trying to add new type [" << name << "] to local scope";

            // when leaving this function all attrs should be consumed
            DLOG_IF(FATAL, !attr_list.empty())
                << "When constructing new struct " << name
                << "its attributes should be consumed";

            return _current_context.lock()->add_struct_type(struct_name);
        }
    }
}

/**
 * ################################################################
 * ###############   Variable related Functions  ##################
 * ################################################################
 */

bool CompilationUnit::has_var(std::string_view name, bool current_scope) {
    return _current_context.lock()->has_var(name, current_scope);
}

void CompilationUnit::addDecl(std::unique_ptr<IDeclStmt> node) {
    // node muse be not a nullptr
    DLOG_ASSERT(node != nullptr) << " Node is nullptr";

    // if we are adding a function Node
    if (node->IsDeclNode()) {
        std::unique_ptr<IDeclStmt> derivedPointer(
            dynamic_cast<IDeclStmt *>(node.release()));
        auto fuc_name = std::string(derivedPointer->name()) +
                        (derivedPointer->has_body() ? "_decl" : "");

        // if we already have the
        auto node_ptr = std::find_if(
            _global_decl.begin(), _global_decl.end(),
            [&fuc_name](const std::pair<std::string, std::unique_ptr<IDeclStmt>>
                            &func) { return func.first == fuc_name; });

        // we need to make sure function/decl name is unique
        if (node_ptr != _global_decl.end()) {
            DLOG(FATAL) << "Decl name: " << fuc_name << " already defined !";
        } else {
            // move ownership of decl node to global_decl
            _global_decl.emplace_back(fuc_name, std::move(derivedPointer));
        }
    } else {
        DLOG(FATAL) << "Unsupported Stmt type, only support decl node";
    }
}
CompilationUnit::~CompilationUnit() = default;
std::string CompilationUnit::GetFileName() const { return _file_name; }

std::list<std::pair<std::string, std::unique_ptr<ast::IDeclStmt>>>
    &CompilationUnit::GetDecls() {
    return _global_decl;
}

}  // namespace hzcc::ast
