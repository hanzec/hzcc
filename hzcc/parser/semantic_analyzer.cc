//
// Created by chen_ on 2022/12/29.
//
#include <absl/strings/str_cat.h>
#include <glog/logging.h>

#include <ext/alloc_traits.h>
#include <list>
#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <utility>

#include "ast/Stmt.h"
#include "ast/cast/Cast.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "enums.h"
#include "options.h"
#include "semantic.h"
#include "utils/logging.h"
#include "utils/status/status.h"
#include "utils/status/statusor.h"

namespace hzcc::semantic {
Status analyzer::visit(hzcc::ast::VarDecl* p_expr) {
    // fist check if type is exist
    HZCC_CHECK_OK_OR_RETURN(p_expr->type_expr()->visit(*this));

    // check duplicate variable, here we only check if the variable is
    // already defined in the current scope. since we can mask the
    // definition of the variable in the parent scope.
    if (_ctx.has_var(p_expr->name(), true)) {
        auto [is_global, line_no] = _ctx.var_def_pos(p_expr->name());
        return CompileError(
            p_expr->loc(),
            (_ctx.at_root() ? "Global variable '" : "Local variable '") +
                std::string(p_expr->name()) +
                (is_global ? "' duplicates global variable declared in"
                           : "' duplicates local variable declared in") +
                " line " + std::to_string(line_no.first));
    }

    // variable cannot be defined as void
    auto curr_type = p_expr->type_expr()->type();
    if (curr_type->is<TypeCategory::Numerical>() &&
        curr_type == ast::GetNumericalTypeOf<PrimitiveType::kVoid>()) {
        return CompileError(
            p_expr->loc(),
            "Variable '" + std::string(p_expr->name()) + "' has type void");
    }

    // add current variable to the _ctx
    _ctx.add_var(p_expr->loc(), curr_type, p_expr->name());

    // check if type is match if there is an initializer
    if (p_expr->has_init()) {
        auto init_type = p_expr->init_expr()->type();
        if (init_type != curr_type) {
            // here we're trying to cast the type to the correct type, since
            // initializer always at right side, we always need rval for this
            // cast
            auto cast_ret = ast::Cast::apply(
                true, std::move(p_expr->init_expr()), curr_type);

            if (!cast_ret.ok()) {
                return CompileError(
                    p_expr->loc(),
                    "Cannot cast from " + init_type->to_str() + " to " +
                        curr_type->to_str() +
                        "since :" + std::string(cast_ret.status().message()));
            }

            p_expr->init_expr() = std::move(cast_ret.value());
        }
    }
    return NoError();
}

Status analyzer::visit(hzcc::ast::ReturnStmt* p_expr) {
    // check return expr first
    HZCC_CHECK_OK_OR_RETURN(p_expr->ret_expr()->visit(*this));

    // return could only be called within function
    if (_ctx.at_root()) {
        return CompileError(p_expr->loc(),
                            "return stmt is not allowed in the root scope");
    }

    // check if return type is match
    if (p_expr->ret_expr()->type() == _ctx.ret_type()) {
        auto return_type = p_expr->ret_expr()->type();
        // cast expr need rvalue to return
        auto ret_expr_ret = ast::Cast::apply(
            true, std::move(p_expr->ret_expr()), _ctx.ret_type().value());

        if (!ret_expr_ret.ok()) {
            return CompileError(p_expr->loc(),
                                "return stmt type is not match, "
                                "require: " +
                                    _ctx.ret_type().value()->to_str() +
                                    ", got: " + return_type->to_str());
        }
        p_expr->ret_expr() = std::move(ret_expr_ret.value());
    }

    return NoError();
}

Status analyzer::visit(hzcc::ast::AssignExpr* p_expr) {
    // check lhs and rhs first
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));

    // LHS has to be an assignable node
    if (!p_expr->lhs()->IsReturnLValue()) {
        return CompileError(p_expr->lhs()->loc(),
                            "Left-hand side is not assignable");
    }

    // LHS cannot be const variable
    if (p_expr->lhs()->type()->is<Qualifier::CONST>()) {
        VLOG(SYNTAX_LOG)
            << "Left hand side [" << p_expr->lhs()->UniqueName()
            << "is not assignable";
        return CompileError(p_expr->lhs()->loc(),
                            "Left hand side is not assignable");
    }

    if (!(p_expr->lhs()->type() == p_expr->rhs()->type())) {
        auto rhs_type = p_expr->rhs()->type();
        auto rhs_ret = ast::Cast::apply(true, std::move(p_expr->rhs()),
                                        p_expr->lhs()->type());
        if (!rhs_ret.ok()) {
            return CompileError(p_expr->rhs()->loc(),
                                "Assignment mismatch " +
                                    p_expr->lhs()->type()->to_str() +
                                    " += " + rhs_type->to_str());
        }
        p_expr->rhs() = std::move(rhs_ret.value());
    }
    return NoError();
}

Status analyzer::visit(hzcc::ast::TernaryExpr* p_expr) {
    // check cond/true/false expr first
    HZCC_CHECK_OK_OR_RETURN(p_expr->cond_expr()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->true_expr()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->false_expr()->visit(*this));

    // todo only allow char
    // true expr and false expr must be same type
    if (!(p_expr->true_expr()->type() == p_expr->false_expr()->type())) {
        auto false_expr_type = p_expr->false_expr()->type();
        auto false_expr_type_ret = ast::Cast::apply(
            true, std::move(p_expr->false_expr()), p_expr->true_expr()->type());
        if (!false_expr_type_ret.ok()) {
            return CompileError(p_expr->loc(),
                                "Expect type " +
                                    p_expr->true_expr()->type()->to_str() +
                                    " but get " + false_expr_type->to_str());
        }
        p_expr->false_expr() = std::move(false_expr_type_ret.value());
    }
    return NoError();
}

Status analyzer::visit(hzcc::ast::LogicalExpr* p_expr) {
    // check lhs and rhs first
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));

    // todo only allow char
    auto lhs_type = p_expr->lhs()->type();
    auto rhs_type = p_expr->rhs()->type();

    // LHS and rhs should not be void
    if (ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type) ||
        ast::IsTypeOf<PrimitiveType::kVoid>(lhs_type)) {
        return CompileError(ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type)
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation void ");
    }

    // for logical operator we only allow numeric type
    if (rhs_type->is<TypeCategory::Numerical>() ||
        rhs_type->is<TypeCategory::Numerical>()) {
        return CompileError(rhs_type->is<TypeCategory::Numerical>()
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation array ");
    }

    if (!(lhs_type == rhs_type)) {
        auto rhs_ret =
            ast::Cast::apply(true, std::move(p_expr->rhs()), lhs_type);
        if (!rhs_ret.ok()) {
            return CompileError(p_expr->rhs()->loc(),
                                "No match for binary operation " +
                                    lhs_type->to_str() + " " +
                                    rhs_type->to_str());
        }
    }
    return NoError();
}
Status analyzer::visit(hzcc::ast::BitwiseExpr* p_expr) {
    // check lhs and rhs first
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));

    // todo only allow char
    auto lhs_type = p_expr->lhs()->type();
    auto rhs_type = p_expr->rhs()->type();

    // LHS and rhs should not be void
    if (ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type) ||
        ast::IsTypeOf<PrimitiveType::kVoid>(lhs_type)) {
        return CompileError(ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type)
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation void ");
    }

    // for logical operator we only allow numeric type
    if (rhs_type->is<TypeCategory::Numerical>() ||
        rhs_type->is<TypeCategory::Numerical>()) {
        return CompileError(rhs_type->is<TypeCategory::Numerical>()
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation array ");
    }

    if (!(lhs_type == rhs_type)) {
        auto rhs_ret =
            ast::Cast::apply(true, std::move(p_expr->rhs()), lhs_type);
        if (!rhs_ret.ok()) {
            return CompileError(p_expr->rhs()->loc(),
                                "No match for binary operation " +
                                    lhs_type->to_str() + " " +
                                    rhs_type->to_str());
        }
    }
    return NoError();
}
Status analyzer::visit(hzcc::ast::RelationalExpr* p_expr) {
    // check lhs and rhs first
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));

    // todo only allow char
    auto lhs_type = p_expr->lhs()->type();
    auto rhs_type = p_expr->rhs()->type();

    // LHS and rhs should not be void
    if (ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type) ||
        ast::IsTypeOf<PrimitiveType::kVoid>(lhs_type)) {
        return CompileError(ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type)
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation void ");
    }

    // for logical operator we only allow numeric type
    if (rhs_type->is<TypeCategory::Numerical>() ||
        rhs_type->is<TypeCategory::Numerical>()) {
        return CompileError(rhs_type->is<TypeCategory::Numerical>()
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation array ");
    }

    if (!(lhs_type == rhs_type)) {
        auto rhs_ret =
            ast::Cast::apply(true, std::move(p_expr->rhs()), lhs_type);
        if (!rhs_ret.ok()) {
            return CompileError(p_expr->rhs()->loc(),
                                "No match for binary operation " +
                                    lhs_type->to_str() + " " +
                                    rhs_type->to_str());
        }
    }
    return NoError();
}
Status analyzer::visit(hzcc::ast::ArithmeticExpr* p_expr) {
    // check lhs and rhs first
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));

    // todo only allow char
    auto lhs_type = p_expr->lhs()->type();
    auto rhs_type = p_expr->rhs()->type();

    // LHS and rhs should not be void
    if (ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type) ||
        ast::IsTypeOf<PrimitiveType::kVoid>(lhs_type)) {
        return CompileError(ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type)
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation void ");
    }

    // for logical operator we only allow numeric type
    if (rhs_type->is<TypeCategory::Numerical>() ||
        rhs_type->is<TypeCategory::Numerical>()) {
        return CompileError(rhs_type->is<TypeCategory::Numerical>()
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation array ");
    }

    // lhs and rhs should not be arrayed
    if (lhs_type->is<TypeCategory::Array>() ||
        rhs_type->is<TypeCategory::Array>()) {
        return CompileError(
            lhs_type->is<TypeCategory::Array>() ? p_expr->lhs()->loc()
                                                : p_expr->rhs()->loc(),
            "No match for binary operation " + lhs_type->to_str() + " " +
                to_string(p_expr->op_type()) + " " + rhs_type->to_str());
    }

    // LHS == RHS
    if (!(lhs_type == rhs_type)) {
        auto rhs_ret =
            ast::Cast::apply(true, std::move(p_expr->rhs()), lhs_type);
        if (!rhs_ret.ok()) {
            return CompileError(p_expr->rhs()->loc(),
                                "No match for binary operation " +
                                    lhs_type->to_str() + " " +
                                    rhs_type->to_str());
        }
    }
    return NoError();
}
Status analyzer::visit(hzcc::ast::UnaryOperator* p_expr) {
    // LHS has to be a variable Node
    if (!p_expr->expr()->IsReturnLValue()) {
        return CompileError(p_expr->loc(), "Expression is not assignable");
    }

    // LHS has to be a variable Node
    if (p_expr->expr()->type()->is<Qualifier::CONST>()) {
        return CompileError(p_expr->expr()->loc(),
                            "cannot set value to const variable");
    }

    return NoError();
}
Status analyzer::visit(hzcc::ast::ArraySubscriptExpr* p_expr) {
    // check subscript and base first
    HZCC_CHECK_OK_OR_RETURN(p_expr->base_expr()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->subscript_expr()->visit(*this));

    // base should be arrayed
    if (!p_expr->base_expr()->type()->is<TypeCategory::Array>()) {
        return CompileError(p_expr->base_expr()->loc(),
                            "Subscripted value is not an array");
    }

    // array subscript should be a numerical type of int
    auto subscript_type = p_expr->subscript_expr()->type();
    if (!ast::IsTypeOf<PrimitiveType::kInt>(subscript_type)) {
        auto index_ret =
            ast::Cast::apply(true, std::move(p_expr->subscript_expr()),
                             ast::GetNumericalTypeOf<PrimitiveType::kInt>());
        if (!index_ret.ok()) {
            return CompileError(p_expr->subscript_expr()->loc(),
                                "Array subscript must be integer");
        }
        p_expr->subscript_expr() = std::move(index_ret.value());
    }
    return NoError();
}

Status analyzer::visit(hzcc::ast::MemberExpr* p_expr) {
    // check base first
    HZCC_CHECK_OK_OR_RETURN(p_expr->base_expr()->visit(*this));

    //  check if the type has this member
    if (!p_expr->base_expr()->type()->is<TypeCategory::Struct>()) {
        return CompileError(p_expr->base_expr()->loc(),
                            "Expect struct type for member "
                            "access");
    }

    // static cast here is safe since we already check the type category
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
    auto type = p_expr->base_expr()->type()->as<ast::StructType>()->field_type(
        p_expr->member_name());
#pragma clang diagnostic pop

    // check member to_str
    if (type == nullptr) {
        return CompileError(
            p_expr->loc(),
            absl::StrCat("Unknown member '", p_expr->member_name(), "' in ",
                         p_expr->base_expr()->type()->to_str()));
    }
    return NoError();
}

Status analyzer::visit(hzcc::ast::FuncCallStmt* p_expr) {
    // check function to_str is matched
    if (!_ctx.has_func(p_expr->name())) {
        return CompileError(
            p_expr->loc(),  // NOLINT
            absl::StrCat("Undefined function '", p_expr->name(), "'"));
    }

    // func definition
    auto& func = _ctx.get_func(p_expr->name());

    // check number of arguments
    if (func->params().size() != p_expr->params().size()) {
        // generate type string first
        std::string type_str = "(";
        for (const auto& arg : func->params()) {
            type_str += absl::StrCat(arg->name(), ", ");
        }
        type_str.replace(type_str.end() - 2, type_str.end(), ")");

        // print out error message
        return CompileError(
            p_expr->loc(),
            absl::StrCat("Parameter mismatch in call to ",
                         func->type()->to_str(), " ", p_expr->name(), type_str,
                         "\n\tExpected ", std::to_string(func->params().size()),
                         ", received ", std::to_string(p_expr->params().size()),
                         " parameters"));
    }

    // check argument types for all arguments
    auto d_arg = func->params().begin();
    auto c_arg = p_expr->params().begin();
    while (d_arg != func->params().end() && c_arg != p_expr->params().end()) {
        if ((*d_arg)->type() != (*c_arg)->type()) {
            auto loc = (*c_arg)->loc();
            auto expr_ret =
                ast::Cast::apply(false, std::move(*c_arg), (*d_arg)->type());
            if (!expr_ret.ok()) {
                return CompileError(
                    loc,
                    absl::StrCat("Expect type ", (*d_arg)->type()->to_str(),
                                 " for argument [", (*d_arg)->name(),
                                 "] but get ", (*c_arg)->type()->to_str()));
            }
            *c_arg = std::move(expr_ret.value());
        }
        ++d_arg;
        ++c_arg;
    }

    // visiting all arguments
    for (const auto& arg : p_expr->params()) {
        HZCC_CHECK_OK_OR_RETURN(arg->visit(*this));
    }

    return NoError();
}

Status analyzer::visit(hzcc::ast::DeclStmt* p_expr) {
    for (const auto& var : p_expr->decls()) {
        auto ret = var->visit(*this);
        if (!ret.ok()) {
            return ret;
        }
    }
    return NoError();
}

Status analyzer::visit(hzcc::ast::ParamVarDecl* p_expr) {
    // check its type node first
    HZCC_CHECK_OK_OR_RETURN(p_expr->type_expr()->visit(*this));

    // we don't allow duplicate type to_str
    if (_ctx.has_var(p_expr->name(), true)) {
        auto [_, line_no] = _ctx.var_def_pos(p_expr->name());
        return CompileError(
            p_expr->loc(),
            absl::StrCat("Duplicate parameter to_str '", p_expr->name(),
                         "'. Previous declaration at ", line_no.first));
    }

    _ctx.add_var(p_expr->loc(), p_expr->type(), p_expr->name());
    return NoError();
}

Status analyzer::visit(hzcc::ast::FuncDeclStmt* p_expr) {
    // handle function body
    if (p_expr->has_body()) {
        _ctx.enter_scope(p_expr->name());

        // add all arguments to the scope
        for (auto& arg : p_expr->params()) {
            HZCC_CHECK_OK_OR_RETURN(arg->visit(*this));
        }

        // check function body
        HZCC_CHECK_OK_OR_RETURN(p_expr->body_stmt()->visit(*this));
    }
    return NoError();
}

void Ctx::add_var(Position pos, ast::QualTypePtr type, std::string_view name) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _compilationUnit->_current_context.lock() != nullptr)
        << " should never call add_var when in root context";
    INTERNAL_LOG_IF(
        FATAL, !_compilationUnit->_current_context.lock()->has_var(name, true))
        << "variable already exists";
#endif

    _compilationUnit->_current_context.lock()->add_var(pos, type, name);
}

std::pair<bool, Position> Ctx::var_def_pos(std::string_view name) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, name.empty() == false)
        << "Variable to_str cannot be empty";
    INTERNAL_LOG_IF(FATAL, has_var(name, false))
        << "Variable " << name << " not found";
#endif

    if (_compilationUnit->_current_context.lock() != nullptr &&
        _compilationUnit->_current_context.lock()->has_var(name, false)) {
        return std::make_pair(
            false,
            _compilationUnit->_current_context.lock()->var_def_pos(name));
    } else {
        DLOG(ERROR) << "Variable " << name << " not found";
        return std::make_pair(false, (Position){0, 0});
    }
}
}  // namespace hzcc::semantic
