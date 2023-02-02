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
    _scoped_symbol_table["ROOT"] = std::make_shared<SymbTbl>(
        std::shared_ptr<QualType>(nullptr), std::shared_ptr<SymbTbl>(nullptr));
    _current_context = _scoped_symbol_table["ROOT"];
}

/** ---------------------------------------------------------------
 * #################   Scope related Functions  ###################
 *--------------------------------------------------------------- */

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
                                  const QualTypePtr &return_type) {
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

std::optional<QualTypePtr> CompilationUnit::ret_type() const {
    if (at_root()) {
        return std::nullopt;
    } else {
        return _current_context.lock()->ret_type();
    }
}

/** ---------------------------------------------------------------
 * #################   Type related Functions  ####################
 *--------------------------------------------------------------- */

bool CompilationUnit::has_type(std::string_view name) {
    if (name.empty()) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
        LOG(FATAL) << "Cannot search for empty type to_str";
#else
        return nullptr;
#endif
    }

    // directly return true if the type is PrimitiveType
    if (parser_common::IsPrimitiveType(name)) {
        return true;
    } else {
        // we fist check our global scope
        auto it = _named_types.find(name);
        if (it != _named_types.end()) {
            return false;
        } else {
            // then check the current scope
            if (_current_context.lock() != nullptr &&
                _current_context.lock()->has_type(name)) {
                return true;
            } else {
                return false;
            }
        }
    }
}

QualTypePtr CompilationUnit::get_type(std::string_view name,
                                      std::list<Qualifier> qualifiers) {
    // directly return true if the type is PrimitiveType
    if (parser_common::IsPrimitiveType(name.data())) {
        if (qualifiers.empty()) {
            return ast::GetNumericalTypeOf(
                parser_common::GetPrimitiveType(name));
        } else {
            return ast::GetNumericalTypeOf(
                       parser_common::GetPrimitiveType(name))
                ->type_of(qualifiers);
        }
    } else {
        // we fist check our global scope
        auto it = _named_types.find(name);
        if (it != _named_types.end()) {
            return std::make_shared<QualType>(it->second, qualifiers);
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

QualTypePtr CompilationUnit::add_struct_type(std::string_view struct_name,
                                             std::list<Qualifier> attr_list) {
    auto name = "struct " + std::string(struct_name);

    // if we are adding global type
    if (_current_context.lock() == nullptr) {
        if (_named_types.find(name) != _named_types.end()) {
            LOG(FATAL) << "Struct " << name << " already exists";
            return nullptr;
        } else {
            DVLOG(AST_LOG)
                << "Add new Struct type [" << name << "] to global scope";
            auto new_type = std::make_shared<ast::StructType>(struct_name);
            _named_types.insert(std::make_pair(name, new_type));

            // when leaving this function all attrs should be consumed
            DLOG_IF(FATAL, !attr_list.empty())
                << "When constructing new struct " << name
                << "its attributes should be consumed";

            return std::make_shared<QualType>(new_type, attr_list);
        }
    } else {
        if (_current_context.lock()->has_type(name)) {
            LOG(FATAL) << "type " << name << " already exists";
            return nullptr;
        } else {
            DVLOG(AST_LOG)
                << "Trying to add new type [" << name << "] to local scope";

            // when leaving this function all attrs should be consumed
            DLOG_IF(FATAL, !attr_list.empty())
                << "When constructing new struct " << name
                << "its attributes should be consumed";

            return std::make_shared<QualType>(
                _current_context.lock()->add_struct_type(struct_name),
                attr_list);
        }
    }
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

std::vector<QualTypePtr> CompilationUnit::func_param_types(
    std::string_view name) {
    if (has_func(name)) {
        std::vector<QualTypePtr> ret;
        for (auto &param : _func_tbl[name]->params()) {
            ret.push_back(param->type());
        }
        return ret;
    } else {
        INTERNAL_LOG(ERROR) << "Function " << name << " does not exist";
        return {};
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
void CompilationUnit::addDecl(std::unique_ptr<IDeclStmt> node) {
    // node muse be not a nullptr
    DLOG_ASSERT(node != nullptr) << " Node is nullptr";

    // if we are adding a function Node
    switch (node->decl_type()) {
        case DeclType::FUNC: {
            // if function def has already been defined
            if (has_func(node->name())) {
                auto &func_old = _func_tbl[node->name()];

                if (node->has_body()) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
                    auto func_new =
                        static_cast<ast::FuncDeclStmt *>(node.get());

                    // if the function body is already defined, then it's a
                    // redefinition
                    INTERNAL_LOG_IF(FATAL, func_old->has_body())
                        << "Function " << node->name() << " already defined !";

                    // check number of arguments
                    INTERNAL_LOG_IF(FATAL, func_old->params().size() !=
                                               func_new->params().size())
                        << "Function " << node->name()
                        << " has different number of arguments !";

                    // then we need to check if the function's signature and
                    // return value is the same
                    auto d_arg = func_old->params().begin();
                    auto c_arg = func_new->params().begin();
                    while (d_arg != func_old->params().end() &&
                           c_arg != func_new->params().end()) {
                        INTERNAL_LOG_IF(FATAL,
                                        (*d_arg)->type() != (*c_arg)->type())
                            << "Function " << node->name()
                            << " has different argument type !";
                        ++d_arg;
                        ++c_arg;
                    }
#endif
                    // otherwise we just add the body to the function
                    func_old->set_body(std::move(
                        static_cast<FuncDeclStmt *>(node.get())->body_stmt()));
                }
                // if the function body is not defined for new function node and
                // the same definition is already defined before then we just
                // ignore the new function node
            }

            // otherwise we just add the function to the global scope
            else {
                // add function to global scope
                _func_tbl.emplace(
                    node->name(),
                    std::make_unique<FuncDeclStmt>(std::move(
                        *static_cast<FuncDeclStmt *>(node.release()))));
            }
        } break;
    }
}
#pragma clang diagnostic pop

CompilationUnit::~CompilationUnit() = default;
std::string CompilationUnit::GetFileName() const { return _file_name; }

}  // namespace hzcc::ast
