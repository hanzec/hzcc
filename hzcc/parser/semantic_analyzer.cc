//
// Created by chen_ on 2022/12/29.
//
#include "ast/Stmt.h"
#include "ast/cast/Cast.h"
#include "ast/expr/Expr.h"
#include "options.h"
#include "semantic.h"
namespace hzcc::semantic {
Status analyzer::visit(hzcc::ast::VarDecl* p_expr) {
    // fist check if type is exist
    HZCC_CHECK_OK_OR_RETURN(p_expr->type_expr()->visit(*this));

    // check duplicate variable, here we only check if the variable is
    // already defined in the current scope. since we can mask the
    // definition of the variable in the parent scope.
    if (_ctx.has_var(p_expr->decl_name(), true)) {
        auto [is_global, line_no] = _ctx.get_var_info(p_expr->decl_name());
        return CompileError(
            p_expr->loc(),
            (_ctx.at_root() ? "Global variable '" : "Local variable '") +
                std::string(p_expr->decl_name()) +
                (is_global ? "' duplicates global variable declared in"
                           : "' duplicates local variable declared in") +
                " line " + std::to_string(line_no.first));
    }

    // variable cannot be defined as void
    auto curr_type = p_expr->type_expr()->retType();
    if (curr_type->is<TypeCategory::kNumerical>() &&
        curr_type == ast::GetNumericalTypeOf<PrimitiveType::kVoid>()) {
        return CompileError(p_expr->loc(),
                            "Variable '" + std::string(p_expr->decl_name()) +
                                "' has type void");
    }

    // add current variable to the _ctx
    _ctx.add_var(p_expr->loc(), curr_type, p_expr->decl_name());

    // check if type is match if there is an initializer
    if (p_expr->has_init()) {
        auto init_type = p_expr->init_expr()->retType();
        if (init_type != curr_type) {
            // here we're trying to cast the type to the correct type, since
            // initializer always at right side, we always need rval for this
            // cast
            auto cast_ret = ast::Cast::apply(
                true, std::move(p_expr->init_expr()), curr_type);

            if (!cast_ret.ok()) {
                return CompileError(
                    p_expr->loc(),
                    "Cannot cast from " + init_type->Name() + " to " +
                        curr_type->Name() +
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
    else if (p_expr->ret_expr()->retType() == _ctx.scope_ret_type()) {
        auto return_type = p_expr->ret_expr()->retType();
        // cast expr need rvalue to return
        auto ret_expr_ret = ast::Cast::apply(
            true, std::move(p_expr->ret_expr()), _ctx.scope_ret_type());

        if (!ret_expr_ret.ok()) {
            return CompileError(p_expr->loc(),
                                "return stmt type is not match, "
                                "require: " +
                                    _ctx.scope_ret_type()->Name() +
                                    ", got: " + return_type->Name());
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
    if (p_expr->lhs()->retType()->is<Attribute::kCONST>()) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Left hand side [" << p_expr->lhs()->UniqueName()
            << "is not assignable";
        return CompileError(p_expr->lhs()->loc(),
                            "Left hand side is not assignable");
    }

    if (!(p_expr->lhs()->retType() == p_expr->rhs()->retType())) {
        auto rhs_type = p_expr->rhs()->retType();
        auto rhs_ret = ast::Cast::apply(true, std::move(p_expr->rhs()),
                                        p_expr->lhs()->retType());
        if (!rhs_ret.ok()) {
            return CompileError(p_expr->rhs()->loc(),
                                "Assignment mismatch " +
                                    p_expr->lhs()->retType()->Name() +
                                    " += " + rhs_type->Name());
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
    if (!(p_expr->true_expr()->retType() == p_expr->false_expr()->retType())) {
        auto false_expr_type = p_expr->false_expr()->retType();
        auto false_expr_type_ret =
            ast::Cast::apply(true, std::move(p_expr->false_expr()),
                             p_expr->true_expr()->retType());
        if (!false_expr_type_ret.ok()) {
            return CompileError(p_expr->loc(),
                                "Expect type " +
                                    p_expr->true_expr()->retType()->Name() +
                                    " but get " + false_expr_type->Name());
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
    auto lhs_type = p_expr->lhs()->retType();
    auto rhs_type = p_expr->rhs()->retType();

    // LHS and rhs should not be void
    if (ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type) ||
        ast::IsTypeOf<PrimitiveType::kVoid>(lhs_type)) {
        return CompileError(ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type)
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation void ");
    }

    // for logical operator we only allow numeric type
    if (rhs_type->is<TypeCategory::kNumerical>() ||
        rhs_type->is<TypeCategory::kNumerical>()) {
        return CompileError(rhs_type->is<TypeCategory::kNumerical>()
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
                                    lhs_type->Name() + " " + rhs_type->Name());
        }
    }
    return NoError();
}
Status analyzer::visit(hzcc::ast::BitwiseExpr* p_expr) {
    // check lhs and rhs first
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));

    // todo only allow char
    auto lhs_type = p_expr->lhs()->retType();
    auto rhs_type = p_expr->rhs()->retType();

    // LHS and rhs should not be void
    if (ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type) ||
        ast::IsTypeOf<PrimitiveType::kVoid>(lhs_type)) {
        return CompileError(ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type)
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation void ");
    }

    // for logical operator we only allow numeric type
    if (rhs_type->is<TypeCategory::kNumerical>() ||
        rhs_type->is<TypeCategory::kNumerical>()) {
        return CompileError(rhs_type->is<TypeCategory::kNumerical>()
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
                                    lhs_type->Name() + " " + rhs_type->Name());
        }
    }
    return NoError();
}
Status analyzer::visit(hzcc::ast::RelationalExpr* p_expr) {
    // check lhs and rhs first
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));

    // todo only allow char
    auto lhs_type = p_expr->lhs()->retType();
    auto rhs_type = p_expr->rhs()->retType();

    // LHS and rhs should not be void
    if (ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type) ||
        ast::IsTypeOf<PrimitiveType::kVoid>(lhs_type)) {
        return CompileError(ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type)
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation void ");
    }

    // for logical operator we only allow numeric type
    if (rhs_type->is<TypeCategory::kNumerical>() ||
        rhs_type->is<TypeCategory::kNumerical>()) {
        return CompileError(rhs_type->is<TypeCategory::kNumerical>()
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
                                    lhs_type->Name() + " " + rhs_type->Name());
        }
    }
    return NoError();
}
Status analyzer::visit(hzcc::ast::ArithmeticExpr* p_expr) {
    // check lhs and rhs first
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));

    // todo only allow char
    auto lhs_type = p_expr->lhs()->retType();
    auto rhs_type = p_expr->rhs()->retType();

    // LHS and rhs should not be void
    if (ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type) ||
        ast::IsTypeOf<PrimitiveType::kVoid>(lhs_type)) {
        return CompileError(ast::IsTypeOf<PrimitiveType::kVoid>(rhs_type)
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation void ");
    }

    // for logical operator we only allow numeric type
    if (rhs_type->is<TypeCategory::kNumerical>() ||
        rhs_type->is<TypeCategory::kNumerical>()) {
        return CompileError(rhs_type->is<TypeCategory::kNumerical>()
                                ? p_expr->lhs()->loc()
                                : p_expr->rhs()->loc(),
                            "No match for binary operation array ");
    }

    // lhs and rhs should not be arrayed
    if (lhs_type->IsArray() || rhs_type->IsArray()) {
        return CompileError(
            lhs_type->is<TypeCategory::kArray>() ? p_expr->lhs()->loc()
                                                 : p_expr->rhs()->loc(),
            "No match for binary operation " + lhs_type->Name() + " " +
                to_string(p_expr->op_type()) + " " + rhs_type->Name());
    }

    // LHS == RHS
    if (!(lhs_type == rhs_type)) {
        auto rhs_ret =
            ast::Cast::apply(true, std::move(p_expr->rhs()), lhs_type);
        if (!rhs_ret.ok()) {
            return CompileError(p_expr->rhs()->loc(),
                                "No match for binary operation " +
                                    lhs_type->Name() + " " + rhs_type->Name());
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
    if (p_expr->expr()->retType()->is<Attribute::kCONST>()) {
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
    if (!p_expr->base_expr()->retType()->IsArray()) {
        return CompileError(p_expr->base_expr()->loc(),
                            "Subscripted value is not an array");
    }

    // array subscript should be a numerical type of int
    auto subscript_type = p_expr->subscript_expr()->retType();
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
    if (!p_expr->base_expr()->retType()->is<TypeCategory::kStruct>()) {
        return CompileError(p_expr->base_expr()->loc(),
                            "Expect struct type for member "
                            "access");
    }

    // static cast here is safe since we already check the type category
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
    auto type =
        static_cast<ast::StructType*>(p_expr->base_expr()->retType().get())
            ->field_type(p_expr->field_name());
#pragma clang diagnostic pop

    // check member name
    if (type == nullptr) {
        return CompileError(
            p_expr->loc(),
            absl::StrCat("Unknown member '", p_expr->field_name(), "' in ",
                         p_expr->base_expr()->retType()->Name()));
    }
    return NoError();
}

Status analyzer::visit(hzcc::ast::FuncCallStmt* p_expr) {
    // check function name is matched
    if (!_ctx.has_func(p_expr->name())) {
        return CompileError(
            p_expr->loc(),  // NOLINT
            absl::StrCat("Unknown function '", p_expr->name(), "'"));
    }

    // get function return type and argument types
    auto [return_type, funcType, line_no] = _ctx.func_def_info(p_expr->name());

    // check number of arguments
    if (funcType.size() != p_expr->args().size()) {
        // generate type string first
        std::string type_str = "(";
        for (const auto& arg : funcType) {
            type_str += arg->Name() + ", ";
        }
        type_str.replace(type_str.end() - 2, type_str.end(), ")");

        // print out error message
        return CompileError(
            p_expr->loc(),
            absl::StrCat("Parameter mismatch in call to ", return_type->Name(),
                         " ", p_expr->name(), type_str, "\n\tExpected ",
                         std::to_string(funcType.size()), ", received ",
                         std::to_string(p_expr->args().size()), " parameters"));
    }

    // check argument types for all arguments
    int i = 0;
    for (auto& expr : p_expr->args()) {
        if (!(*expr->retType() == *funcType.front())) {
            auto arg_type = expr->retType();
            auto expr_ret =
                ast::Cast::apply(false, std::move(expr), funcType.front());
            if (!expr_ret.ok()) {
                return CompileError(
                    expr->loc(),
                    absl::StrCat("Expect type ", funcType.front()->Name(),
                                 " for argument ", i, " but get ",
                                 arg_type->Name()));
            }
            expr = std::move(expr_ret.value());
        }
        i++;
        funcType.pop_front();
    }
    return NoError();
}

Status analyzer::visit(hzcc::ast::DeclStmt* p_expr) {
    for (const auto& var : p_expr->decl_list()) {
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

    // we don't allow duplicate type name
    if (_ctx.has_var(p_expr->decl_name(), true)) {
        auto [_, line_no] = _ctx.get_var_info(p_expr->decl_name());
        return CompileError(
            p_expr->loc(),
            absl::StrCat("Duplicate parameter name '", p_expr->decl_name(),
                         "'. Previous declaration at ", line_no.first));
    }

    _ctx.add_var(p_expr->loc(), p_expr->declType(), p_expr->decl_name());
    return NoError();
}

Status analyzer::visit(hzcc::ast::FuncDeclStmt* p_expr) {
    // function name cannot duplicate with any other variable name
    if (!Options::Global_allow_same_name_for_func_val &&
        _ctx.has_var(p_expr->decl_name(), true)) {
        return CompileError(
            p_expr->loc(),
            absl::StrCat("Function '", p_expr->decl_name(),
                         "' already defined as global variable"));
    }

    // check function name is redefinition or not only when the function body is
    // exits
    if (p_expr->has_body() && _ctx.has_func(p_expr->decl_name()) &&
        _ctx.has_func_body(p_expr->decl_name())) {
        return CompileError(p_expr->loc(),
                            absl::StrCat("Function '", p_expr->decl_name(),
                                         "' already defined"));
    }

    // function with same name must match previous definition
    std::list<ast::TypePtr> arg_types;
    if (_ctx.has_func(p_expr->decl_name())) {
        auto [func_type, func_type_attributes, line_no] =
            _ctx.func_def_info(p_expr->decl_name());

        // compare argument number
        if (func_type_attributes.size() != p_expr->params().size()) {
            return CompileError(
                p_expr->loc(),
                absl::StrCat("Function '", p_expr->decl_name(),
                             "' already defined with different argument "
                             "number from previous definition in line" +
                                 std::to_string(line_no.first)));
        }

        // compare return type
        if (!(func_type == p_expr->type_expr()->retType())) {
            return CompileError(
                p_expr->loc(),
                absl::StrCat("Function ", p_expr->decl_name(),
                             " already defined with ", func_type->Name(),
                             " return type in line ",
                             std::to_string(line_no.first)));
        }

        // compare all argument type
        for (const auto& param : p_expr->params()) {
            if (!(param->declType() == func_type_attributes.front())) {
                return CompileError(
                    param->loc(),
                    absl::StrCat("Function ", p_expr->decl_name(),
                                 " already defined with different argument "
                                 "type in line ",
                                 std::to_string(line_no.first)));
            }
            func_type_attributes.pop_front();
            arg_types.push_back(param->declType());
        }
    }

    // add function definition to _ctx
    _ctx.add_func(p_expr->loc(), p_expr->decl_name(),
                  p_expr->type_expr()->retType(), arg_types);

    // handle function body
    if (p_expr->has_body()) {
        _ctx.enter_scope(p_expr->decl_name());

        // add all arguments to the scope
        for (auto& arg : p_expr->params()) {
            HZCC_CHECK_OK_OR_RETURN(arg->visit(*this));
        }

        // check function body
        HZCC_CHECK_OK_OR_RETURN(p_expr->body_stmt()->visit(*this));
    }
    return NoError();
}
}  // namespace hzcc::semantic
