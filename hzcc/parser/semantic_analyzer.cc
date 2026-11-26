//
// Created by chen_ on 2022/12/29.
//
#include <absl/cleanup/cleanup.h>
#include <absl/strings/str_cat.h>
#include <glog/logging.h>

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
namespace {
/** @brief Describe an operand for a diagnostic, including its name when unresolved. */
std::string operand_type(const ast::Expr& operand) {
    if (const auto type = operand.type()) return type->to_str();
    if (const auto* ref = dynamic_cast<const ast::DeclRefExpr*>(&operand)) {
        return absl::StrCat("<unresolved type of '", ref->var_name(), "'>");
    }
    if (const auto* call = dynamic_cast<const ast::FuncCallStmt*>(&operand)) {
        return absl::StrCat("<unresolved return type of '", call->name(), "'>");
    }
    return absl::StrCat("<unresolved type of ", operand.NodeName(), ">");
}
}  // namespace

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
    if (!curr_type)
        return CompileError(p_expr->loc(),
                            absl::StrCat("Variable '", p_expr->name(),
                                         "' has an unresolved declared type"));
    if (ast::IsTypeOf<PrimitiveType::kVoid>(curr_type)) {
        return CompileError(
            p_expr->loc(),
            "Variable '" + std::string(p_expr->name()) + "' has type void");
    }

    // check if type is match if there is an initializer
    if (p_expr->has_init()) {
        HZCC_CHECK_OK_OR_RETURN(p_expr->init_expr()->visit(*this));
        // here we're trying to cast the type to the correct type, since
        // initializer always at right side, we always need rval for this
        // cast
        auto cast = ast::Cast::apply_to(true, p_expr->init_expr(), curr_type);
        if (!cast.ok()) {
            return CompileError(p_expr->init_expr()->loc(),
                                absl::StrCat("Initializer for variable '", p_expr->name(),
                                             "': ", cast.message()));
        }
    }
    // add current variable to the _ctx
    _ctx.add_var(p_expr->loc(), curr_type, p_expr->name());
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
    if (const auto required = _ctx.ret_type()) {
        // cast expr need rvalue to return
        auto cast = ast::Cast::apply_to(true, p_expr->ret_expr(), *required);
        if (!cast.ok()) {
            return CompileError(p_expr->ret_expr()->loc(),
                                absl::StrCat("Return type mismatch: ", cast.message()));
        }
    }

    return NoError();
}

Status analyzer::visit(hzcc::ast::AssignExpr* p_expr) {
    // check lhs and rhs first
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));

    const auto op = to_string(p_expr->op_type());
    // LHS has to be an assignable node
    // LHS cannot be const variable
    const auto dest = p_expr->lhs()->type();
    if (!p_expr->lhs()->IsReturnLValue() || !dest || dest->is<Qualifier::CONST>()) {
        return CompileError(
            p_expr->lhs()->loc(),
            absl::StrCat("Left-hand side of '", operand_type(*p_expr->lhs()), " ", op,
                         " ", operand_type(*p_expr->rhs()), "' ",
                         dest ? "is not assignable" : "has an unresolved type"));
    }
    auto cast = ast::Cast::apply_to(true, p_expr->rhs(), dest);
    if (!cast.ok()) {
        return CompileError(
            p_expr->rhs()->loc(),
            absl::StrCat("Assignment mismatch for '", op, "': ", cast.message()));
    }
    return NoError();
}

Status analyzer::visit(hzcc::ast::TernaryExpr* p_expr) {
    // check cond/true/false expr first
    HZCC_CHECK_OK_OR_RETURN(p_expr->cond_expr()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->true_expr()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->false_expr()->visit(*this));

    // true expr and false expr must be same type
    const auto target = p_expr->true_expr()->type();
    auto cast = ast::Cast::apply_to(true, p_expr->false_expr(), target);
    if (!cast.ok()) {
        return CompileError(
            target ? p_expr->false_expr()->loc() : p_expr->true_expr()->loc(),
            absl::StrCat("Incompatible branches of '?:': ", cast.message()));
    }
    return NoError();
}

namespace {
/** @brief Check both numerical operands and widen the lower-ranked operand in place. */
Status check_binary(analyzer& visitor, ast::OperatorBase& expr, std::string_view op,
                    bool integral_only) {
    // check lhs and rhs first
    HZCC_CHECK_OK_OR_RETURN(expr.lhs()->visit(visitor));
    HZCC_CHECK_OK_OR_RETURN(expr.rhs()->visit(visitor));
    const auto left = expr.lhs()->type();
    const auto right = expr.rhs()->type();
    auto reject = [&](const ast::Expr& operand, std::string_view reason) {
        return CompileError(
            operand.loc(),
            absl::StrCat("No match for binary operation '", operand_type(*expr.lhs()),
                         " ", op, " ", operand_type(*expr.rhs()),
                         "': ", &operand == expr.lhs().get() ? "left" : "right",
                         " operand ", reason));
    };
    for (const auto* operand : {expr.lhs().get(), expr.rhs().get()}) {
        const auto& type = operand == expr.lhs().get() ? left : right;
        if (!type) return reject(*operand, "has an unresolved type");
        // LHS and rhs should not be void
        if (ast::IsTypeOf<PrimitiveType::kVoid>(type))
            return reject(*operand, "has type void");
        if (!type->is<TypeCategory::Numerical>())
            return reject(*operand, "must have numerical type");
        if (integral_only && !type->as<ast::NumericalType>()->is_integer()) {
            return reject(*operand, "must have integer type");
        }
    }
    if (*left != *right) {
        const bool widen_left = left->as<ast::NumericalType>()->widening_rank() <
                                right->as<ast::NumericalType>()->widening_rank();
        auto& narrower = widen_left ? expr.lhs() : expr.rhs();
        auto cast = ast::Cast::apply_to(true, narrower, widen_left ? right : left);
        if (!cast.ok())
            return reject(*narrower, absl::StrCat("conversion failed: ", cast.message()));
    }
    return NoError();
}
}  // namespace

Status analyzer::visit(hzcc::ast::LogicalExpr* p_expr) {
    return check_binary(*this, *p_expr, to_string(p_expr->op_type()), false);
}

Status analyzer::visit(hzcc::ast::BitwiseExpr* p_expr) {
    return check_binary(*this, *p_expr, to_string(p_expr->op_type()), true);
}

Status analyzer::visit(hzcc::ast::RelationalExpr* p_expr) {
    return check_binary(*this, *p_expr, to_string(p_expr->op_type()), false);
}

Status analyzer::visit(hzcc::ast::ArithmeticExpr* p_expr) {
    return check_binary(*this, *p_expr, to_string(p_expr->op_type()),
                        p_expr->op_type() == ArithmeticType::MOD);
}
Status analyzer::visit(hzcc::ast::UnaryOperator* p_expr) {
    HZCC_CHECK_OK_OR_RETURN(p_expr->expr()->visit(*this));
    switch (p_expr->GetUnaryType()) {
        case UnaryType::PRE_INC:
        case UnaryType::PRE_DEC:
        case UnaryType::POST_INC:
        case UnaryType::POST_DEC:
            break;
        default:
            return NoError();
    }
    const auto type = p_expr->expr()->type();
    if (!type || !p_expr->expr()->IsReturnLValue() || type->is<Qualifier::CONST>()) {
        const bool increment = p_expr->GetUnaryType() == UnaryType::PRE_INC ||
                               p_expr->GetUnaryType() == UnaryType::POST_INC;
        return CompileError(p_expr->expr()->loc(),
                            absl::StrCat("Operand of '", increment ? "++" : "--",
                                         "' has type '", operand_type(*p_expr->expr()),
                                         "' and must be a modifiable lvalue"));
    }

    return NoError();
}
Status analyzer::visit(hzcc::ast::ArraySubscriptExpr* p_expr) {
    // check subscript and base first
    HZCC_CHECK_OK_OR_RETURN(p_expr->base_expr()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->subscript_expr()->visit(*this));

    // base should be arrayed
    if (!p_expr->base_expr()->type() ||
        !p_expr->base_expr()->type()->is<TypeCategory::Array>()) {
        return CompileError(
            p_expr->base_expr()->loc(),
            absl::StrCat("Subscripted value of type '",
                         operand_type(*p_expr->base_expr()), "' is not an array"));
    }

    // array subscript should be a numerical type of int
    auto subscript_type = p_expr->subscript_expr()->type();
    if (!ast::IsTypeOf<PrimitiveType::kInt>(subscript_type)) {
        auto index_ret =
            ast::Cast::apply_to(true, p_expr->subscript_expr(),
                                ast::GetNumericalTypeOf<PrimitiveType::kInt>());
        if (!index_ret.ok()) {
            return CompileError(
                p_expr->subscript_expr()->loc(),
                absl::StrCat("Array subscript must be integer: ", index_ret.message()));
        }
    }
    return NoError();
}

Status analyzer::visit(hzcc::ast::MemberExpr* p_expr) {
    // check base first
    HZCC_CHECK_OK_OR_RETURN(p_expr->base_expr()->visit(*this));

    //  check if the type has this member
    if (!p_expr->base_expr()->type() ||
        !p_expr->base_expr()->type()->is<TypeCategory::Struct>()) {
        return CompileError(
            p_expr->base_expr()->loc(),
            absl::StrCat("Expected struct for access to member '", p_expr->member_name(),
                         "', got '", operand_type(*p_expr->base_expr()), "'"));
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
    auto* func = _ctx.get_func(p_expr->name());

    // check number of arguments
    if (func->params().size() != p_expr->params().size()) {
        // generate type string first
        std::string type_str = "(";
        for (const auto& arg : func->params()) {
            if (type_str.size() > 1) type_str += ", ";
            type_str += arg->name();
        }
        type_str += ")";

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
        // visit each argument before checking its type
        HZCC_CHECK_OK_OR_RETURN((*c_arg)->visit(*this));
        auto cast = ast::Cast::apply_to(false, *c_arg, (*d_arg)->type());
        if (!cast.ok()) {
            return CompileError((*c_arg)->loc(),
                                absl::StrCat("In call to '", p_expr->name(),
                                             "', argument for parameter '",
                                             (*d_arg)->name(), "': ", cast.message()));
        }
        ++d_arg;
        ++c_arg;
    }

    p_expr->set_return_type(func->type());
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
        auto leave = absl::MakeCleanup([this] { _ctx.leave_scope(); });

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
    LOG_IF(FATAL, _compilationUnit->_current_context.expired())
        << "missing active symbol table";
    LOG_IF(FATAL, _compilationUnit->_current_context.lock()->has_var(name, true))
        << "variable already exists";
#endif

    _compilationUnit->_current_context.lock()->add_var(pos, type, name);
}

std::pair<bool, Position> Ctx::var_def_pos(std::string_view name) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    LOG_IF(FATAL, name.empty()) << "Variable to_str cannot be empty";
    LOG_IF(FATAL, !has_var(name, false)) << "Variable " << name << " not found";
#endif

    if (_compilationUnit->_current_context.lock() != nullptr &&
        _compilationUnit->_current_context.lock()->has_var(name, false)) {
        return std::make_pair(
            at_root(), _compilationUnit->_current_context.lock()->var_def_pos(name));
    } else {
        DLOG(ERROR) << "Variable " << name << " not found";
        return std::make_pair(false, (Position){0, 0});
    }
}
}  // namespace hzcc::semantic
