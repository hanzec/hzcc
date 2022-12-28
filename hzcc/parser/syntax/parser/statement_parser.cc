//
// Created by chen_ on 2022/3/24.
//
#include <list>

#include "ast/CompilationUnit.h"
#include "ast/cast/CastTool.h"
#include "ast/expr/Expr.h"
#include "parser/syntax/parser/utils/macros.h"
#include "parser/syntax/parser_factory.h"
#include "parser/syntax/utils/common_utils.h"
#include "syntax_parser.h"
namespace hzcc::syntax::parser {
Statement::Statement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::Stmt>(),
                 TypeNameUtil::name_pretty<ast::Stmt>()) {}

StatusOr<ast::StmtPtr> Statement::parse_impl(TokenList& tokens, Ctx& context) {
    switch (tokens.peek().Type()) {
        case TokenType::kSemiColon:
            return std::make_unique<ast::EmptyStmt>();
        case TokenType::kStruct:
            if (tokens.peek3().Type() == TokenType::kLBrace) {
                return ParserFactory::ParseAST<ast::DeclStmt>(tokens, context);
            } else {
                return ParseCommaExpr(context, tokens);
            }
        case TokenType::kReturn: {
            auto token = tokens.pop();
            if (tokens.peek().Type() != TokenType::kSemiColon) {
                HZCC_CHECK_OR_ASSIGN(ret_expr,                         // NOLINT
                                     ParseCommaExpr(context, tokens))  // NOLINT
                if (context.isRoot()) {
                    return syntax::utils::TokenErr(
                        token,
                        "return stmt is not allowed in the root "
                        "scope");
                } else if (*ret_expr->retType() == *context.GetReturnType()) {
                    auto return_type = ret_expr->retType();
                    // cast expr need rvalue to return
                    auto ret_expr_ret = ast::CastTool::apply(
                        true, std::move(ret_expr), context.GetReturnType());

                    if (!ret_expr_ret.ok()) {
                        return syntax::utils::TokenErr(
                            token,
                            "return stmt type is not match, "
                            "require: " +
                                context.GetReturnType()->Name() +
                                ", got: " + return_type->Name());
                    }
                    ret_expr = std::move(ret_expr_ret.value());
                }
                return std::make_unique<ast::ReturnStmt>(token.Location(),
                                                         std::move(ret_expr));
            } else {
                DVLOG(SYNTAX_LOG_LEVEL)
                    << "ignore ';' since return stmt is empty";
                return std::make_unique<ast::ReturnStmt>(
                    token.Location(), std::make_unique<ast::EmptyExpr>());
            }
        }
        case TokenType::kIf:
            return ParserFactory::ParseAST<ast::IfStmt>(tokens, context);
        case TokenType::kDo:
            return ParserFactory::ParseAST<ast::DoStmt>(tokens, context);
        case TokenType::kFor:
            return ParserFactory::ParseAST<ast::ForStmt>(tokens, context);
        case TokenType::kWhile:
            return ParserFactory::ParseAST<ast::WhileStmt>(tokens, context);
        case TokenType::kBreak:
            if (WithinLoop()) {
                tokens.pop();
                return std::make_unique<ast::BreakStmt>(
                    tokens.peek().Location());
            } else {
                return syntax::utils::TokenErr(tokens,
                                               "break stmt not within a loop");
            }
        case TokenType::kContinue:
            if (WithinLoop()) {
                tokens.pop();
                return std::make_unique<ast::ContinueStmt>(
                    tokens.peek().Location());
            } else {
                return syntax::utils::TokenErr(
                    tokens, "continue stmt not within a loop");
            }
        case TokenType::kType:
        case TokenType::kIdentity: {
            if (context.hasType(tokens.peek().Value())) {
                return ParserFactory::ParseAST<ast::VarDecl>(tokens, context);
            }
        }
        default:
            return ParseCommaExpr(context, tokens);
    }
}

StatusOr<ast::ExprPtr> Statement::ParseCommaExpr(Ctx& context,
                                                 TokenList& tokens) {
    // parsing left-hand side of expression
    HZCC_CHECK_OR_ASSIGN(lhs,                               // NOLINT
                         ParseAssignExpr(context, tokens))  // NOLINT

    // if we have a comma, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == TokenType::kComma) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                              // NOLINT
                             ParseCommaExpr(context, tokens))  // NOLINT
        return std::make_unique<ast::CommaExpr>(next.Location(), std::move(lhs),
                                                std::move(rhs));
    } else {
        return lhs;
    }
}

StatusOr<ast::ExprPtr> Statement::ParseAssignExpr(Ctx& context,
                                                  TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                                    // NOLINT
                         ParseConditionalExpr(context, tokens))  // NOLINT

    // if we have a assign cast, we should parse the right-hand side
    if (tokens.peek().Type() == TokenType::kAssign ||
        tokens.peek().Type() == TokenType::kAddAssign ||
        tokens.peek().Type() == TokenType::kSubAssign ||
        tokens.peek().Type() == TokenType::kMulAssign ||
        tokens.peek().Type() == TokenType::kDivAssign ||
        tokens.peek().Type() == TokenType::kModAssign ||
        tokens.peek().Type() == TokenType::kLShiftAssign ||
        tokens.peek().Type() == TokenType::kRShiftAssign ||
        tokens.peek().Type() == TokenType::kAndAssign ||
        tokens.peek().Type() == TokenType::kXorAssign ||
        tokens.peek().Type() == TokenType::kOrAssign) {
        auto assign_type = tokens.peek();
        tokens.pop();

        HZCC_CHECK_OR_ASSIGN(rhs,                               // NOLINT
                             ParseAssignExpr(context, tokens))  // NOLINT

        // LHS has to be an assignable node
        if (!lhs->IsReturnLValue()) {
            return syntax::utils::TokenErr(assign_type,
                                           "Left-hand side is not assignable");
        }

        // LHS cannot be const variable
        if (lhs->retType()->is<ast::Attribute::kCONST>()) {
            VLOG(SYNTAX_LOG_LEVEL) << "Left hand side [" << lhs->UniqueName()
                                   << "is not assignable";
            return syntax::utils::TokenErr(assign_type,
                                           "Left hand side is not assignable");
        }

        // lhs and rhs should not be arrayed
        if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
            return syntax::utils::TokenErr(
                assign_type, "No match for binary operation " +
                                 lhs->retType()->Name() + " " +
                                 to_string(assign_type.Type()) + " " +
                                 rhs->retType()->Name());
        }

        if (!(*lhs->retType() == *rhs->retType())) {
            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(assign_type,
                                               "Assignment mismatch " +
                                                   lhs->retType()->Name() +
                                                   " += " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::AssignExpr>(
            assign_type.Location(), assign_type.Value(), std::move(lhs),
            std::move(rhs));
    } else {
        return lhs;
    }
}

// TODO: add syntax for conditional expression
StatusOr<ast::ExprPtr> Statement::ParseConditionalExpr(Ctx& context,
                                                       TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                                  // NOLINT
                         ParseLogicalOrExpr(context, tokens))  // NOLINT

    if (tokens.peek().Type() == TokenType::kQuestionMark) {
        auto cond_loc = tokens.peek().Location();
        tokens.pop();  // pop '?'

        // parse true expression

        // todo only allow char
        HZCC_CHECK_OR_ASSIGN(true_expr,                               // NOLINT
                             ParseConditionalExpr(context, tokens);)  // NOLINT

        // pop ':'
        HZCC_CheckAndConsume_ReturnErr(TokenType::kColon, tokens);

        // parse false expression
        auto next = tokens.peek();
        HZCC_CHECK_OR_ASSIGN(false_expr,                             // NOLINT
                             ParseConditionalExpr(context, tokens))  // NOLINT

        if (!(*true_expr->retType() == *false_expr->retType())) {
            auto rhs_type = true_expr->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(false_expr),
                                                true_expr->retType());
            if (false_expr == nullptr) {
                return syntax::utils::TokenErr(
                    next, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::TernaryExpr>(
            cond_loc, std::move(lhs), std::move(true_expr), std::move(rhs));
    } else {
        return lhs;
    }
}

StatusOr<ast::ExprPtr> Statement::ParseLogicalOrExpr(Ctx& context,
                                                     TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                                   // NOLINT
                         ParseLogicalAndExpr(context, tokens))  // NOLINT

    // if we have a logical or, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == TokenType::kLogicalOr) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                                  // NOLINT
                             ParseLogicalOrExpr(context, tokens))  // NOLINT

        // todo only allow char
        if (!(*lhs->retType() == *rhs->retType())) {
            // LHS and rhs should not be void
            if (lhs->retType()->Name(true) == "void" ||
                rhs->retType()->Name() == "void") {
                return syntax::utils::TokenErr(
                    next, "No match for binary operation void " +
                              to_string(next.Type()) + " void");
            }

            // lhs and rhs should not be arrayed
            if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
                return syntax::utils::TokenErr(
                    next, "No match for binary operation " +
                              lhs->retType()->Name() + " " +
                              to_string(next.Type()) + " " +
                              rhs->retType()->Name());
            }

            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(
                    next, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::LogicalExpr>(
            next.Value(), std::move(lhs), std::move(rhs), next.Location());
    } else {
        return lhs;
    }
}

StatusOr<ast::ExprPtr> Statement::ParseLogicalAndExpr(Ctx& context,
                                                      TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                                  // NOLINT
                         ParseBitwiseOrExpr(context, tokens))  // NOLINT

    // if we have a logical and, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == TokenType::kLogicalAnd) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                                   // NOLINT
                             ParseLogicalAndExpr(context, tokens))  // NOLINT

        // todo only allow char
        if (!(*lhs->retType() == *rhs->retType())) {
            // LHS and rhs should not be void
            if (lhs->retType()->Name(true) == "void" ||
                rhs->retType()->Name() == "void") {
                return syntax::utils::TokenErr(
                    next, "No match for binary operation void " +
                              to_string(next.Type()) + " void");
            }

            // lhs and rhs should not be arrayed
            if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
                return syntax::utils::TokenErr(
                    next, "No match for binary operation " +
                              lhs->retType()->Name() + " " +
                              to_string(next.Type()) + " " +
                              rhs->retType()->Name());
            }

            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(
                    next, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::LogicalExpr>(
            next.Value(), std::move(lhs), std::move(rhs), next.Location());
    } else {
        return lhs;
    }
}

StatusOr<ast::ExprPtr> Statement::ParseBitwiseOrExpr(Ctx& context,
                                                     TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                                   // NOLINT
                         ParseBitwiseXorExpr(context, tokens))  // NOLINT

    // if we have a bitwise or, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == TokenType::kBitwiseOr) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                                  // NOLINT
                             ParseBitwiseOrExpr(context, tokens))  // NOLINT

        if (!(*lhs->retType() == *rhs->retType())) {
            // LHS and rhs should not be void
            if (lhs->retType()->Name(true) == "void" ||
                rhs->retType()->Name() == "void") {
                return syntax::utils::TokenErr(
                    next, "No match for binary operation void " +
                              to_string(next.Type()) + " void");
            }

            // lhs and rhs should not be arrayed
            if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
                return syntax::utils::TokenErr(
                    next, "No match for binary operation " +
                              lhs->retType()->Name() + " " +
                              to_string(next.Type()) + " " +
                              rhs->retType()->Name());
            }

            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(
                    next, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::BitwiseExpr>(
            next.Location(), next.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

StatusOr<ast::ExprPtr> Statement::ParseBitwiseXorExpr(Ctx& context,
                                                      TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                                   // NOLINT
                         ParseBitwiseAndExpr(context, tokens))  // NOLINT

    // if we have a bitwise xor, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == TokenType::kBitwiseXor) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                                   // NOLINT
                             ParseBitwiseXorExpr(context, tokens))  // NOLINT

        if (!(*lhs->retType() == *rhs->retType())) {
            // LHS and rhs should not be void
            if (lhs->retType()->Name(true) == "void" ||
                rhs->retType()->Name() == "void") {
                return syntax::utils::TokenErr(
                    next, "No match for binary operation void " +
                              to_string(next.Type()) + " void");
            }

            // lhs and rhs should not be arrayed
            if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
                return syntax::utils::TokenErr(
                    next, "No match for binary operation " +
                              lhs->retType()->Name() + " " +
                              to_string(next.Type()) + " " +
                              rhs->retType()->Name());
            }

            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(
                    next, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::BitwiseExpr>(
            next.Location(), next.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

StatusOr<ast::ExprPtr> Statement::ParseBitwiseAndExpr(Ctx& context,
                                                      TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                                 // NOLINT
                         ParseEqualityExpr(context, tokens))  // NOLINT

    // if we have a bitwise and, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == TokenType::kBitwiseAnd) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                                   // NOLINT
                             ParseBitwiseAndExpr(context, tokens))  // NOLINT

        if (!(*lhs->retType() == *rhs->retType())) {
            // LHS and rhs should not be void
            if (lhs->retType()->Name(true) == "void" ||
                rhs->retType()->Name() == "void") {
                return syntax::utils::TokenErr(
                    next, "No match for binary operation void " +
                              to_string(next.Type()) + " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
                return syntax::utils::TokenErr(
                    next, "No match for binary operation " +
                              lhs->retType()->Name() + " " +
                              to_string(next.Type()) + " " +
                              rhs->retType()->Name())
            }

            // trying to cast rhs to lhs
            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(
                    next, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }
        return std::make_unique<ast::BitwiseExpr>(
            next.Location(), next.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

StatusOr<ast::ExprPtr> Statement::ParseEqualityExpr(Ctx& context,
                                                    TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                                   // NOLINT
                         ParseRelationalExpr(context, tokens))  // NOLINT

    // if we have a equality, we should parse the next expression
    if (tokens.peek().Type() == TokenType::kEqual ||
        tokens.peek().Type() == TokenType::kNotEqual) {
        auto type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                                 // NOLINT
                             ParseEqualityExpr(context, tokens))  // NOLINT

        if (!(*lhs->retType() == *rhs->retType())) {
            // LHS and rhs should not be void
            if (lhs->retType()->Name(true) == "void" ||
                rhs->retType()->Name() == "void") {
                return syntax::utils::TokenErr(
                    type, "No match for binary operation void " +
                              to_string(type.Type()) + " void");
            }

            // lhs and rhs should not be arrayed
            if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
                return syntax::utils::TokenErr(
                    type, "No match for binary operation " +
                              lhs->retType()->Name() + " " +
                              to_string(type.Type()) + " " +
                              rhs->retType()->Name());
            }

            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(
                    type, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::RelationalExpr>(
            type.Location(), type.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

StatusOr<ast::ExprPtr> Statement::ParseRelationalExpr(Ctx& context,
                                                      TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                              // NOLINT
                         ParseShiftExpr(context, tokens))  // NOLINT

    // if we have a relational, we should parse the next
    // expression
    if (tokens.peek().Type() == TokenType::kLess ||
        tokens.peek().Type() == TokenType::kGreater ||
        tokens.peek().Type() == TokenType::kLessEqual ||
        tokens.peek().Type() == TokenType::kGreaterEqual) {
        auto type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                                   // NOLINT
                             ParseRelationalExpr(context, tokens))  // NOLINT

        if (!(*lhs->retType() == *rhs->retType())) {
            // LHS and rhs should not be void
            if (lhs->retType()->Name(true) == "void" ||
                rhs->retType()->Name() == "void") {
                return syntax::utils::TokenErr(
                    type, "No match for binary operation void " +
                              to_string(type.Type()) + " void");
            }

            // lhs and rhs should not be arrayed
            if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
                return syntax::utils::TokenErr(
                    type, "No match for binary operation " +
                              lhs->retType()->Name() + " " +
                              to_string(type.Type()) + " " +
                              rhs->retType()->Name());
            }

            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(
                    type, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::RelationalExpr>(
            type.Location(), type.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
    return nullptr;
}

StatusOr<ast::ExprPtr> Statement::ParseShiftExpr(Ctx& context,
                                                 TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                                 // NOLINT
                         ParseAdditiveExpr(context, tokens))  // NOLINT

    // if we have a shift, we should parse the next expression
    if (tokens.peek().Type() == TokenType::kLeftShift ||
        tokens.peek().Type() == TokenType::kRightShift) {
        auto type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                              // NOLINT
                             ParseShiftExpr(context, tokens))  // NOLINT

        if (!(*lhs->retType() == *rhs->retType())) {
            // LHS and rhs should not be void
            if (lhs->retType()->Name(true) == "void" ||
                rhs->retType()->Name() == "void") {
                return syntax::utils::TokenErr(
                    type, "No match for binary operation void " +
                              to_string(type.Type()) + " void");
            }

            // lhs and rhs should not be arrayed
            if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
                return syntax::utils::TokenErr(
                    type, "No match for binary operation " +
                              lhs->retType()->Name() + " " +
                              to_string(type.Type()) + " " +
                              rhs->retType()->Name());
            }

            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(
                    type, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::BitwiseExpr>(
            type.Location(), type.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

StatusOr<ast::ExprPtr> Statement::ParseAdditiveExpr(Ctx& context,
                                                    TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                                       // NOLINT
                         ParseMultiplicativeExpr(context, tokens))  // NOLINT

    // if we have a additive, we should parse the next
    // expression
    if (tokens.peek().Type() == TokenType::kAdd ||
        tokens.peek().Type() == TokenType::kSub) {
        auto type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                                 // NOLINT
                             ParseAdditiveExpr(context, tokens))  // NOLINT

        // LHS and rhs should not be void
        if (lhs->retType()->Name(true) == "void" ||
            rhs->retType()->Name() == "void") {
            return syntax::utils::TokenErr(
                type, "No match for binary operation void " +
                          to_string(type.Type()) + " void");
        }

        // lhs and rhs should not be arrayed
        if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
            return syntax::utils::TokenErr(type,
                                           "No match for binary operation " +
                                               lhs->retType()->Name() + " " +
                                               to_string(type.Type()) + " " +
                                               rhs->retType()->Name());
        }

        // LHS == RHS
        if (!(*lhs->retType() == *rhs->retType())) {
            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(
                    type, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::ArithmeticExpr>(
            type.Location(), type.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
    return nullptr;
}

StatusOr<ast::ExprPtr> Statement::ParseMultiplicativeExpr(Ctx& context,
                                                          TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(lhs,                              // NOLINT
                         ParseUnaryExpr(context, tokens))  // NOLINT

    // if we have a multiplicative, we should parse the next
    // expression
    if (tokens.peek().Type() == TokenType::kMul ||
        tokens.peek().Type() == TokenType::kDiv ||
        tokens.peek().Type() == TokenType::kMod) {
        auto type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(
            rhs,                                       // NOLINT
            ParseMultiplicativeExpr(context, tokens))  // NOLINT

        // LHS and rhs should not be void
        if (lhs->retType()->Name(true) == "void" ||
            rhs->retType()->Name() == "void") {
            return syntax::utils::TokenErr(
                type, "No match for binary operation void " +
                          to_string(type.Type()) + " void");
        }

        // lhs and rhs should not be arrayed
        if (lhs->retType()->IsArray() || rhs->retType()->IsArray()) {
            return syntax::utils::TokenErr(type,
                                           "No match for binary operation " +
                                               lhs->retType()->Name() + " " +
                                               to_string(type.Type()) + " " +
                                               rhs->retType()->Name());
        }

        // LHS == RHS
        if (!(*lhs->retType() == *rhs->retType())) {
            auto rhs_type = rhs->retType();
            auto rhs_ret = ast::CastTool::apply(std::move(rhs), lhs->retType());
            if (rhs == nullptr) {
                return syntax::utils::TokenErr(
                    type, "Expect type " + lhs->retType()->Name() +
                              " but get " + rhs_type->Name());
            }
        }

        return std::make_unique<ast::ArithmeticExpr>(
            type.Location(), type.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

StatusOr<ast::ExprPtr> Statement::ParseUnaryExpr(Ctx& context,
                                                 TokenList& tokens) {
    // some of unary expression does not have LHS
    if (tokens.peek().Type() == TokenType::kSub ||
        tokens.peek().Type() == TokenType::kLogicalNot ||
        tokens.peek().Type() == TokenType::kReference ||
        tokens.peek().Type() == TokenType::kBitWiseNot ||
        tokens.peek().Type() == TokenType::kDereference) {
        auto type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs,                              // NOLINT
                             ParseUnaryExpr(context, tokens))  // NOLINT

        return std::make_unique<ast::UnaryOperator>(
            type.Location(), type.Value(), std::move(rhs));
    }

    // for cast expression, we should parse the next expression
    else if (tokens.peek().Type() == TokenType::kLParentheses &&
             tokens.size() >= 2 && context.hasType(tokens.peek2().Value())) {
        auto cast_loc = tokens.peek();
        tokens.pop();  // consume the '('

        // get the type of the cast expression
        auto type_name = ParseTypeName(tokens, context);

        if (type_name.empty()) {
            return syntax::utils::TokenErr(tokens.peek(), "Expected type name");
            return nullptr;
        }

        auto [argument_type, name_token] =
            ParseTypeDecl(TokenListToString(type_name), tokens, context);

        // consume the ')'
        HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens);

        auto rhs = ParseUnaryExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for cast expression failed";
            return nullptr;
        }

        // RetType == RHS
        if ((argument_type->IsArray() && !rhs->retType()->IsArray()) ||
            (!argument_type->IsArray() && rhs->retType()->IsArray()) ||
            (argument_type->Name() == "void" ||
             rhs->retType()->Name() == "void")) {
            return syntax::utils::TokenErr(cast_loc,
                                           "Cannot cast expression of type " +
                                               rhs->retType()->Name() + " to " +
                                               argument_type->Name());
        }

        if (rhs->retType()->IsConst()) {
            return std::make_unique<ast::CastExpr>(
                cast_loc.Location(), argument_type->GetConstType(),
                std::move(rhs));
        } else {
            return std::make_unique<ast::CastExpr>(
                cast_loc.Location(), argument_type, std::move(rhs));
        }
    }

    // sizeof cast
    else if (tokens.peek().Type() == TokenType::kSizeOf) {
        auto size_of_loc = tokens.peek().Location();
        tokens.pop();  // consume the sizeof

        // consume the '('
        HZCC_CheckAndConsume_ReturnErr(TokenType::kLParentheses, tokens);

        auto expr = ParseUnaryExpr(context, tokens);
        if (expr == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [EXPR] for sizeof expression failed";
            return nullptr;
        }

        // consume the ')'
        HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens);

        return std::make_unique<ast::SizeofExpr>(std::move(expr), size_of_loc);
    }

    // selfInc/selfDec and otherwise
    else {
        // since self inc/dec could before the variable or after
        // the variable we need to check the next token
        if (tokens.peek().Type() == TokenType::kSelfIncrement ||
            tokens.peek().Type() == TokenType::kSelfDecrement) {
            auto type = tokens.peek();
            tokens.pop();

            // parsing RHS
            auto rhs = ParseUnaryExpr(context, tokens);
            if (rhs == nullptr) {
                VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for unary "
                                          "expression failed";
                return nullptr;
            }

            // LHS has to be a variable Node
            if (!rhs->IsReturnLValue()) {
                return syntax::utils::TokenErr(type,
                                               "cannot add value to rvalue");
            }

            // LHS has to be a variable Node
            if (rhs->retType()->IsConst()) {
                return syntax::utils::TokenErr(
                    type, "cannot set value to const variable");
            }

            return std::make_unique<ast::UnaryOperator>(
                type.Location(), type.Value(), std::move(rhs));
        } else {
            auto lhs = ParseAccessExpr(context, tokens);
            if (lhs == nullptr) {
                VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for unary "
                                          "expression failed";
                return nullptr;
            }

            // check the next token
            if (tokens.peek().Type() == TokenType::kSelfIncrement) {
                // LHS has to be a variable Node
                if (!lhs->IsReturnLValue()) {
                    return syntax::utils::TokenErr(
                        tokens, "Expression is not assignable")
                }

                // LHS has to be a variable Node
                if (lhs->retType()->IsConst()) {
                    return syntax::utils::TokenErr(
                        tokens, "cannot set value to const variable")
                }

                auto type = tokens.pop();
                return std::make_unique<ast::UnaryOperator>(
                    type.Location(), type.Value(), std::move(lhs));
            } else if (tokens.peek().Type() == TokenType::kSelfDecrement) {
                // LHS has to be a variable Node
                if (!lhs->IsReturnLValue()) {
                    return syntax::utils::TokenErr(
                        tokens, "Expression is not assignable")
                }

                // LHS has to be a variable Node
                if (lhs->retType()->IsConst()) {
                    return syntax::utils::TokenErr(
                        tokens, "cannot set value to const variable");
                }
                auto type = tokens.pop();
                return std::make_unique<ast::UnaryOperator>(
                    type.Location(), type.Value(), std::move(lhs));
            } else {
                return std::move(lhs);
            }
        }
    }
}

StatusOr<ast::ExprPtr> Statement::ParseAccessExpr(Ctx& context,
                                                  TokenList& tokens) {
    HZCC_CHECK_OR_ASSIGN(current_expr,                       // NOLINT
                         ParsePostfixExpr(context, tokens))  // NOLINT

    // variable accessor
    if (tokens.peek().Type() == TokenType::kDot ||
        tokens.peek().Type() == TokenType::kArrow) {
        do {
            if (tokens.peek().Type() == TokenType::kArrow ||
                tokens.peek().Type() == TokenType::kDot) {
                // we need to know is it a pointer or not
                bool isPtr = tokens.pop().Type() == TokenType::kArrow;

                // accessor should not be a keyword
                if (tokens.peek().Type() != TokenType::kIdentity) {
                    return syntax::utils::TokenErr(tokens, "Expect identifier");
                }

                // to new a variable node
                auto memberName = tokens.pop();

                //  check if the type has this member
                if (!current_expr->retType()->IsStruct()) {
                    return syntax::utils::TokenErr(
                        tokens,
                        "Expect struct type for member "
                        "access")
                }

                auto type = dynamic_cast<ast::StructType*>(
                                current_expr->retType().get())
                                ->GetChild(memberName.Value());

                // check member name
                if (type == nullptr) {
                    return syntax::utils::TokenErr(
                        tokens, "Unknown member '" + memberName.Value() +
                                    "' in " + current_expr->retType()->Name());
                }

                current_expr = std::make_unique<ast::AccessExpr>(
                    isPtr, memberName.Location(), memberName.Value(),
                    std::move(current_expr));
            } else {
                // consume the ']'
                auto start = tokens.peek();
                HZCC_CheckAndConsume_ReturnErr(TokenType::kLBracket, tokens);

                // consume the index
                HZCC_CHECK_OR_ASSIGN(index_expr,                       // NOLINT
                                     ParseCommaExpr(context, tokens))  // NOLINT

                // check type
                if (!current_expr->retType()->IsArray()) {
                    return syntax::utils::TokenErr(
                        tokens, "Expect array type for array accessor");
                }

                // consume the ']'

                HZCC_CheckAndConsume_ReturnErr(TokenType::kRBracket, tokens);
                current_expr = std::make_unique<ast::ArraySubscriptExpr>(
                    start.Location(), std::move(current_expr),
                    std::move(index_expr));
            }
        } while (tokens.peek().Type() == TokenType::kDot ||
                 tokens.peek().Type() == TokenType::kArrow ||
                 tokens.peek().Type() == TokenType::kLBracket);

        return current_expr;
    } else {
        return current_expr;
    }
}

StatusOr<ast::ExprPtr> Statement::ParsePostfixExpr(Ctx& context,
                                                   TokenList& tokens) {
    // first handle priority expression ("()")
    if (tokens.peek().Type() == TokenType::kLParentheses) {
        tokens.pop();  // consume the '('

        HZCC_CHECK_OR_ASSIGN(expr,                             // NOLINT
                             ParseCommaExpr(context, tokens))  // NOLINT

        // consume the ')'
        HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens);

        return std::move(expr);
    }

    // handle access/array
    else if (tokens.peek().Type() == TokenType::kIdentity &&
             !context.hasType(tokens.peek().Value())) {
        auto name = tokens.pop();

        // array
        if (tokens.peek().Type() == TokenType::kLBracket) {
            auto start = tokens.pop();  // consume the '['

            // array accessor should be integer
            HZCC_CHECK_OR_ASSIGN(index,                            // NOLINT
                                 ParseCommaExpr(context, tokens))  // NOLINT

            auto int_type = ast::Type::GetTypeOf("int", {});
            if (!(*int_type == *index->retType())) {
                auto index_type = index->retType();
                index = ast::Stmt::CastTo(std::move(index), int_type);
                if (index == nullptr) {
                    return syntax::utils::TokenErr(
                        start, "Expect type int but get " + index_type->Name());
                }
            }

            // consume the ']'
            HZCC_CheckAndConsume_ReturnErr(TokenType::kRBracket, tokens);

            if (!context.hasVariable(name.Value(), false)) {
                return syntax::utils::TokenErr(
                    name, "Undefined variable: " + name.Value());
            }

            auto type = context.getVariableType(name.Value());

            if (type->IsArray()) {
                return std::make_unique<ast::ArraySubscriptExpr>(
                    start.Location(),
                    std::make_unique<ast::DeclRefExpr>(name.Location(),
                                                       name.Value(), type),
                    std::move(index));
            } else {
                return syntax::utils::TokenErr(
                    name, "Expect array type for array accessor");
            }
        }

        // function caller
        else if (tokens.peek().Type() == TokenType::kLParentheses) {
            tokens.pop();  // consume the '('

            // check function name is matched
            if (!context.hasFunction(name.Value())) {
                return syntax::utils::TokenErr(
                    name, "Function " + name.Value() + " not found");
            }

            // next token should not be ", "
            if (tokens.peek().Type() == TokenType::kComma) {
                return syntax::utils::TokenErr(tokens, "term expected");
            }

            // parse arguments
            std::list<std::unique_ptr<ast::Expr>> args_expr;
            while (tokens.peek().Type() != TokenType::kRParentheses) {
                auto arg_token = tokens.peek();
                HZCC_CHECK_OR_ASSIGN(
                    arg,                               // NOLINT
                    ParseAssignExpr(context, tokens))  // NOLINT

                // consume the ',' if have other arguments
                if (tokens.peek().Type() != TokenType::kRParentheses) {
                    HZCC_CheckAndConsume_ReturnErr(TokenType::kComma, tokens);

                    // after consume the ',', we should have a
                    // valid argument
                    if (tokens.peek().Type() == TokenType::kRParentheses) {
                        return syntax::utils::TokenErr(tokens.peek(),
                                                       "term expected");
                    }
                }
                args_expr.push_back(std::move(arg));
            }

            // get function return type and argument types
            auto [return_type, funcType, line_no] =
                context.getFuncRetAndArgType(name.Value());

            // function call need to have same number of arguments

            if (!context.hasFunction(name.Value())) {
                return syntax::utils::TokenErr(
                    name, "Function " + name.Value() + " not found");
            }

            // check number of arguments
            if (funcType.size() != args_expr.size()) {
                // generate type string first
                std::string type_str = "(";
                for (const auto& arg : funcType) {
                    type_str += arg->Name() + ", ";
                }
                type_str.replace(type_str.end() - 2, type_str.end(), ")");

                // print out error message
                return syntax::utils::TokenErr(
                    name, "Parameter mismatch in call to " +
                              return_type->Name() + " " + name.Value() +
                              type_str + "\n\tExpected " +
                              std::to_string(funcType.size()) + ", received " +
                              std::to_string(args_expr.size()) + " parameters");
            }

            // check argument types for all arguments
            for (auto& expr : args_expr) {
                if (!(*expr->retType() == *funcType.front())) {
                    auto arg_type = expr->retType();
                    expr = ast::Stmt::CastTo(std::move(expr), funcType.front());
                    if (expr == nullptr) {
                        return syntax::utils::TokenErr(
                            name, "Expect type " + funcType.front()->Name() +
                                      " for argument " +
                                      std::to_string(args_expr.size() + 1) +
                                      " but get " + arg_type->Name());
                    }
                }
                funcType.pop_front();
            }

            // consume the ')'
            HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens);

            return std::make_unique<ast::FuncCallStmt>(
                name.Location(), return_type, name.Value(),
                std::move(args_expr));
        }

        if (context.hasVariable(name.Value(), false)) {
            return std::make_unique<ast::DeclRefExpr>(
                name.Location(), name.Value(),
                context.getVariableType(name.Value()));
        } else {
            return syntax::utils::TokenErr(
                name, "Undefined variable: " + name.Value());
        }
    }

    // const values (string literal will handle differently
    // since it could be concat)
    else if (tokens.peek().Type() == TokenType::kChar ||
             tokens.peek().Type() == TokenType::kInteger ||
             tokens.peek().Type() == TokenType::kReal_number) {
        auto value = tokens.peek();

        switch (tokens.pop().Type()) {
            case TokenType::kChar:
                return std::make_unique<ast::LiteralExpr>(
                    ast::kLiteralType_Char, value.Location(), value.Value());
            case TokenType::kInteger:
                return std::make_unique<ast::LiteralExpr>(
                    ast::kLiteralType_Integer, value.Location(), value.Value());
            case TokenType::kReal_number:
                return std::make_unique<ast::LiteralExpr>(
                    ast::kLiteralType_Real_number, value.Location(),
                    value.Value());
            default:
                LOG(FATAL) << "Unreachable code: \n"
                           << "\t Current ast:\n";
                return nullptr;
        }
    }

    // handle string literal
    else if (tokens.peek().Type() == TokenType::kString) {
        auto value = tokens.pop();
        auto value_str = value.Value();

        while (tokens.peek().Type() == TokenType::kString) {
            value_str += tokens.pop().Value();
        }

        return std::make_unique<ast::LiteralExpr>(ast::kLiteralType_String,
                                                  value.Location(), value_str);
    }

    // if we can detect types
    else {
        if (tokens.peek().Type() == TokenType::kElse) {
            return syntax::utils::TokenErr(tokens,
                                           "Else has to be followed by if");
        } else {
            return syntax::utils::TokenErr(tokens, "term expected");
        }
    }
}

}  // namespace hzcc::syntax::parser
