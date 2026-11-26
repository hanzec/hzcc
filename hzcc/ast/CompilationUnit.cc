#include "CompilationUnit.h"

#include <glog/logging.h>

#include <algorithm>
#include <ostream>

#include "ast/Stmt.h"
#include "ast/type/Type.h"
#include "options.h"
#include "parser/common/name_utils.h"

namespace hzcc::ast {
CompilationUnit::CompilationUnit(std::string file_name)
    : _file_name(std::move(file_name)) {
    _scoped_symbol_table[""] = std::make_shared<SymbTbl>(
        std::shared_ptr<QualType>(nullptr), std::shared_ptr<SymbTbl>(nullptr));
    _current_context = _scoped_symbol_table[""];
}

/** ---------------------------------------------------------------
 * #################   Scope related Functions  ###################
 *--------------------------------------------------------------- */

void CompilationUnit::leave_scope() {
    if (at_root()) {
        DLOG(FATAL) << "cannot leave scope, already at root";
    } else {
        _current_context = _current_context.lock()->getUpperScope();
    }
}

bool CompilationUnit::at_root() const {
    return _current_context.lock()->ret_type() == nullptr;
}

void CompilationUnit::enter_scope(std::string_view name) {
    if (!name.empty()) {
        LOG_IF(FATAL, _scoped_symbol_table.find(name) == _scoped_symbol_table.end())
            << "cannot enter scope " << name << ", not found";
        _current_context = _scoped_symbol_table[name];
    } else {
        LOG_IF(FATAL, at_root()) << "cannot enter unnamed scope, if already at root";

        auto new_context = _current_context.lock()->enter_scope();
        _current_context = new_context;
    }
}

void CompilationUnit::create_func(std::string_view name, const QualTypePtr &return_type) {
    DLOG_IF(WARNING, return_type == nullptr) << "scope " << name << " has no return type";
    if (!at_root()) {
        DLOG(FATAL) << "Only root context can enter a named scope";
    } else {
        _scoped_symbol_table[name] =
            std::make_shared<SymbTbl>(return_type, _current_context);
        _current_context = _scoped_symbol_table[name];
    }
}

std::optional<QualTypePtr> CompilationUnit::ret_type() const {
    if (at_root()) {
        return std::nullopt;
    } else {
        return _current_context.lock()->ret_type();
    }
}

/** ---------------------------------------------------------------
 * #################   type related Functions  ####################
 *--------------------------------------------------------------- */
bool CompilationUnit::has_type(std::string_view name, bool current_scope) {
    if (name.empty()) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG(FATAL) << "Cannot search for an empty type name";
#endif
        return false;
    }

    // directly return true if the type is PrimitiveType
    return parser_common::is_primitive_type(name) ||
           (_named_types.contains(name) && (!current_scope || at_root())) ||
           _current_context.lock()->has_type(name, current_scope);
}

QualTypePtr CompilationUnit::get_type(std::string_view name,
                                      const std::vector<Qualifier> &qualifiers) {
    // return the primitive type directly
    if (parser_common::is_primitive_type(name)) {
        auto type = ast::GetNumericalTypeOf(parser_common::GetPrimitiveType(name));
        return qualifiers.empty() ? type : type->type_of(qualifiers);
    }

    // Check the current scope before falling back to our global scope.
    if (auto local = _current_context.lock()->get_type(name, qualifiers)) {
        return local;
    }
    auto it = _named_types.find(name);
    return it == _named_types.end() ? nullptr
                                    : std::make_shared<QualType>(it->second, qualifiers);
}

QualTypePtr CompilationUnit::add_struct_type(std::string_view struct_name,
                                             const std::vector<Qualifier> &attr_list) {
    auto name = "struct " + std::string(struct_name);

    // if we are adding global type
    if (at_root()) {
        auto base = std::make_shared<ast::StructType>(struct_name);
        const auto inserted = _named_types.emplace(name, base).second;
        LOG_IF(FATAL, !inserted) << "Struct " << name << " already exists";
        return std::make_shared<QualType>(base, attr_list);
    }
    return std::make_shared<QualType>(
        _current_context.lock()->add_struct_type(struct_name), attr_list);
}

/** ---------------------------------------------------------------
 * ###############   Variable related Functions  ##################
 *--------------------------------------------------------------- */
bool CompilationUnit::has_var(std::string_view name, bool current_scope) {
    return _current_context.lock()->has_var(name, current_scope);
}

/** ---------------------------------------------------------------
 * ## Function related Functions                               ###
 * -------------------------------------------------------------- */

std::vector<QualTypePtr> CompilationUnit::func_param_types(std::string_view name) {
    if (has_func(name)) {
        std::vector<QualTypePtr> ret;
        for (auto &param : _func_tbl[name]->params()) {
            ret.push_back(param->type());
        }
        return ret;
    } else {
        LOG(ERROR) << "Function " << name << " does not exist";
        return {};
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
void CompilationUnit::addDecl(std::unique_ptr<IDeclStmt> node) {
    // node must not be a nullptr
    DCHECK(node != nullptr) << "Declaration node is nullptr";

    // if we are adding a function Node
    if (node->decl_type() == DeclType::FUNC) {
        FuncDeclStmtPtr function(static_cast<FuncDeclStmt*>(node.release()));
        auto [it, inserted] = _func_tbl.try_emplace(function->name(), nullptr);
        if (inserted) {
            // add function to global scope
            it->second = std::move(function);
        } else if (function->has_body()) {
            auto& previous = *it->second;
#ifdef HZCC_ENABLE_RUNTIME_CHECK
            // if the function body is already defined, then it's a
            // redefinition
            LOG_IF(FATAL, previous.has_body())
                << "Function " << function->name() << " already defined";

            // check number of arguments
            LOG_IF(FATAL, previous.params().size() != function->params().size())
                << "Function " << function->name() << " has different number of arguments";

            // then check if the parameter types are the same
            auto old_param = previous.params().begin();
            for (const auto& param : function->params()) {
                LOG_IF(FATAL, *(*old_param)->type() != *param->type())
                    << "Function " << function->name() << " has different argument type";
                ++old_param;
            }
#endif
            // otherwise we add the parameter names and body to the function
            previous.params() = std::move(function->params());
            previous.set_body(std::move(function->body_stmt()));
        }
        // If the new function node has no body, ignore a repeated prototype.
        return;
    } else if (node->decl_type() == DeclType::COMB_VAR) {
        for (const auto& var : static_cast<DeclStmt*>(node.get())->decls()) {
            if (!has_var(var->name(), true)) {
                auto type = var->type();
                _current_context.lock()->add_var(var->loc(), type, var->name());
            }
        }
    }
    _declarations.push_back(std::move(node));
}
#pragma clang diagnostic pop

CompilationUnit::~CompilationUnit() = default;
std::string CompilationUnit::GetFileName() const { return _file_name; }

}  // namespace hzcc::ast
