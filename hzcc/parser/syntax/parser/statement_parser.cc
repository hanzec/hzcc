//
// Created by chen_ on 2022/3/24.
//
#include <glog/logging.h>

#include <iterator>
#include <list>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "ast/CompilationUnit.h"
#include "ast/Stmt.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "enums.h"
#include "parser/common/Token.h"
#include "parser/common/name_utils.h"
#include "parser/common/token_type.h"
#include "parser/parser.h"
#include "parser/syntax/common_utils.h"
#include "parser_factory.h"
#include "syntax_parser.h"
#include "utils/logging.h"
#include "utils/status/status.h"
#include "utils/status/statusor.h"
#include "utils/type_name_utils.h"

namespace hzcc::syntax::parser {
namespace {
/** @brief Diagnose a missing operand at its delimiter, or at the operator on EOF. */
Status RequireOperand(const Token& op, TokenList& tokens) {
    if (!tokens.empty()) {
        switch (tokens.peek().type()) {
            case TokenType::SemiColon:
            case TokenType::RBrace:
            case TokenType::RParentheses:
            case TokenType::RBracket:
            case TokenType::Comma:
            case TokenType::Colon:
                break;
            default:
                return NoError();
        }
    }
    return utils::TokenErr(
        tokens.empty() ? op : tokens.peek(),
        "term expected\n        Expected an operand after '" + op.to_str() + "'");
}

/** @brief Recognize a parenthesized type name before choosing cast or expression grammar.
 */
bool StartsTypeName(const TokenList& tokens, ast::CompilationUnit& ctx) {
    if (tokens.peek().type() != TokenType::LParentheses || tokens.size() < 2)
        return false;
    // A qualifier after '(' can only introduce a type name, even if its name is unknown.
    auto opening = tokens.begin();
    while (is_qualifier(opening->type())) ++opening;
    auto after_opening = std::next(opening);
    if (after_opening != tokens.end() && is_qualifier(after_opening->type())) return true;
    const auto& next = tokens.peek2();
    return next.type() == TokenType::Struct || next.type() == TokenType::Enum ||
           next.type() == TokenType::Union || ctx.has_type(next.to_str());
}
}  // namespace

Statement::Statement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::Stmt>(),
                 TypeNameUtil::name_pretty<ast::Stmt>()) {}

StatusOr<ast::StmtPtr> Statement::parse_impl(SyntaxCtx ctx, TokenList& tokens) {
    const auto first = tokens.peek();
    if (is_type_specifier(first.type()) || first.type() == TokenType::Struct ||
        first.type() == TokenType::Enum || first.type() == TokenType::Union ||
        (first.type() == TokenType::kIdentity && ctx->has_type(first.to_str()))) {
        return Parser::Parse<ast::IDeclStmt>(ctx, tokens);
    }
    switch (first.type()) {
        case TokenType::SemiColon:
            tokens.pop();
            return std::make_unique<ast::EmptyStmt>();
        case TokenType::LBrace:
            return utils::ParseBodyStatement(ctx, tokens);
        case TokenType::Return: {
            tokens.pop();
            ast::ExprPtr value = std::make_unique<ast::EmptyExpr>();
            if (!tokens.empty() && tokens.peek().type() != TokenType::SemiColon &&
                tokens.peek().type() != TokenType::RBrace) {
                HZCC_CHECK_OR_ASSIGN(expr, ParseCommaExpr(tokens, ctx));
                value = std::move(expr);
            }
            HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::SemiColon, tokens, first));
            return std::make_unique<ast::ReturnStmt>(first.loc(), std::move(value));
        }
        case TokenType::If:
            return Parser::Parse<ast::IfStmt>(ctx, tokens);
        case TokenType::Do:
            return Parser::Parse<ast::DoStmt>(ctx, tokens);
        case TokenType::For:
            return Parser::Parse<ast::ForStmt>(ctx, tokens);
        case TokenType::While:
            return Parser::Parse<ast::WhileStmt>(ctx, tokens);
        case TokenType::Break:
        case TokenType::Continue:
            if (!ctx->within_loop()) {
                return utils::TokenErr(first, first.to_str() + " stmt not within a loop");
            }
            tokens.pop();
            HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::SemiColon, tokens, first));
            if (first.type() == TokenType::Break) {
                return std::make_unique<ast::BreakStmt>(first.loc());
            }
            return std::make_unique<ast::ContinueStmt>(first.loc());
        default:
            HZCC_CHECK_OR_ASSIGN(expr, ParseCommaExpr(tokens, ctx));
            HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::SemiColon, tokens, first));
            return expr;
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::StmtPtr> ExpressionParser::parse_impl(SyntaxCtx ctx, TokenList& tokens) {
    return Statement::ParseCommaExpr(tokens, ctx);
}
#pragma clang diagnostic pop

StatusOr<ast::ExprPtr> Statement::ParseCommaExpr(TokenList& tokens,
                                                 const SyntaxCtx& context) {
    // parsing left-hand side of expression
    HZCC_CHECK_OR_ASSIGN(lhs, ParseAssignExpr(tokens, context));

    // if we have a comma, we should parse the next expression
    while (!tokens.empty() && tokens.peek().type() == TokenType::Comma) {
        auto comma = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(comma, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseAssignExpr(tokens, context));
        lhs =
            std::make_unique<ast::CommaExpr>(comma.loc(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseAssignExpr(TokenList& tokens,
                                                  const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseConditionalExpr(tokens, ctx));

    // if we have a assign cast, we should parse the right-hand side
    if (!tokens.empty() && (tokens.peek().type() == TokenType::Assign ||
                            tokens.peek().type() == TokenType::kAddAssign ||
                            tokens.peek().type() == TokenType::kSubAssign ||
                            tokens.peek().type() == TokenType::kMulAssign ||
                            tokens.peek().type() == TokenType::kDivAssign ||
                            tokens.peek().type() == TokenType::kModAssign ||
                            tokens.peek().type() == TokenType::kLShiftAssign ||
                            tokens.peek().type() == TokenType::kRShiftAssign ||
                            tokens.peek().type() == TokenType::kAndAssign ||
                            tokens.peek().type() == TokenType::kXorAssign ||
                            tokens.peek().type() == TokenType::kOrAssign)) {
        auto assign_type = tokens.pop();
        if (!lhs->IsReturnLValue()) {
            return utils::TokenErr(assign_type, "Left-hand side is not assignable");
        }

        HZCC_CHECK_OK_OR_RETURN(RequireOperand(assign_type, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseAssignExpr(tokens, ctx));
        return std::make_unique<ast::AssignExpr>(
            assign_type.loc(), assign_type.to_str(), std::move(lhs),
            std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseConditionalExpr(TokenList& tokens,
                                                       const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseLogicalOrExpr(tokens, ctx));

    if (!tokens.empty() && tokens.peek().type() == TokenType::QuestionMark) {
        auto question = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(question, tokens));

        // parse true expression
        HZCC_CHECK_OR_ASSIGN(true_expr, ParseCommaExpr(tokens, ctx));

        // pop ':'
        const auto colon = tokens.empty() ? question : tokens.peek();
        HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::Colon, tokens, question));
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(colon, tokens));

        // parse false expression
        HZCC_CHECK_OR_ASSIGN(false_expr, ParseConditionalExpr(tokens, ctx));
        return std::make_unique<ast::TernaryExpr>(
            question.loc(), std::move(lhs), std::move(true_expr), std::move(false_expr));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseLogicalOrExpr(TokenList& tokens,
                                                     const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseLogicalAndExpr(tokens, ctx));

    // if we have a logical or, we should parse the next expression
    while (!tokens.empty() && (tokens.peek().type() == TokenType::kLogicalOr)) {
        auto op = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseLogicalAndExpr(tokens, ctx));
        lhs = std::make_unique<ast::LogicalExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseLogicalAndExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseBitwiseOrExpr(tokens, ctx));

    // if we have a logical and, we should parse the next expression
    while (!tokens.empty() && (tokens.peek().type() == TokenType::kLogicalAnd)) {
        auto op = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseBitwiseOrExpr(tokens, ctx));
        lhs = std::make_unique<ast::LogicalExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseBitwiseOrExpr(TokenList& tokens,
                                                     const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseBitwiseXorExpr(tokens, ctx));

    // if we have a bitwise or, we should parse the next expression
    while (!tokens.empty() && (tokens.peek().type() == TokenType::BitwiseOr)) {
        auto op = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseBitwiseXorExpr(tokens, ctx));

        lhs = std::make_unique<ast::BitwiseExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseBitwiseXorExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseBitwiseAndExpr(tokens, ctx));

    // if we have a bitwise xor, we should parse the next expression
    while (!tokens.empty() && (tokens.peek().type() == TokenType::BitwiseXor)) {
        auto op = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseBitwiseAndExpr(tokens, ctx));
        lhs = std::make_unique<ast::BitwiseExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseBitwiseAndExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseEqualityExpr(tokens, ctx));

    while (!tokens.empty() && (tokens.peek().type() == TokenType::BitwiseAnd)) {
        auto op = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseEqualityExpr(tokens, ctx));
        lhs = std::make_unique<ast::BitwiseExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseEqualityExpr(TokenList& tokens,
                                                    const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseRelationalExpr(tokens, ctx));

    while (!tokens.empty() && (tokens.peek().type() == TokenType::EQ ||
                               tokens.peek().type() == TokenType::NE)) {
        auto op = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseRelationalExpr(tokens, ctx));
        lhs = std::make_unique<ast::RelationalExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseRelationalExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseShiftExpr(tokens, ctx));

    while (!tokens.empty() && (tokens.peek().type() == TokenType::Less ||
                               tokens.peek().type() == TokenType::Greater ||
                               tokens.peek().type() == TokenType::LE ||
                               tokens.peek().type() == TokenType::GE)) {
        auto op = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseShiftExpr(tokens, ctx));
        lhs = std::make_unique<ast::RelationalExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseShiftExpr(TokenList& tokens,
                                                 const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseAdditiveExpr(tokens, ctx));

    // if we have a shift, we should parse the next expression
    while (!tokens.empty() && (tokens.peek().type() == TokenType::kLeftShift ||
                               tokens.peek().type() == TokenType::kRightShift)) {
        auto op = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseAdditiveExpr(tokens, ctx));
        lhs = std::make_unique<ast::BitwiseExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseAdditiveExpr(TokenList& tokens,
                                                    const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseMultiplicativeExpr(tokens, ctx));

    // if we have a additive, we should parse the next
    // expression
    while (!tokens.empty() && (tokens.peek().type() == TokenType::Add ||
                               tokens.peek().type() == TokenType::Sub)) {
        auto op = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseMultiplicativeExpr(tokens, ctx));
        lhs = std::make_unique<ast::ArithmeticExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseMultiplicativeExpr(
    TokenList& tokens, const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseCastExpr(tokens, ctx));

    // if we have a multiplicative, we should parse the next
    // expression
    while (!tokens.empty() && (tokens.peek().type() == TokenType::Mul ||
                               tokens.peek().type() == TokenType::Div ||
                               tokens.peek().type() == TokenType::Mod)) {
        auto op = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseCastExpr(tokens, ctx));
        lhs = std::make_unique<ast::ArithmeticExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    }
    return lhs;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::TypeProxyExprPtr> Statement::ParseTypeName(TokenList& tokens,
                                                         const SyntaxCtx& ctx) {
    auto type = tokens.peek();

    // handle base types
    if (type.type() == TokenType::kIdentity && !ctx->has_type(type.to_str())) {
        return utils::TokenErr(type, "unknown type '" + type.to_str() + "'");
    }
    ast::QualTypePtr type_ptr;
    if (type.type() == TokenType::Struct || type.type() == TokenType::Enum ||
        type.type() == TokenType::Union) {
        tokens.pop();
        if (tokens.empty() || tokens.peek().type() != TokenType::kIdentity) {
            return utils::TokenErr(tokens.empty() ? type : tokens.peek(),
                                   "Expected identifier after '" + type.to_str() + "'");
        }
        auto tag = tokens.pop();
        auto name = type.to_str() + " " + tag.to_str();
        if (!ctx->has_type(name)) {
            return utils::TokenErr(tag, "unknown type '" + name + "'");
        }
        std::vector<Qualifier> qualifiers;
        for (const auto& attr : tokens.take_leading_attributes()) {
            qualifiers.push_back(to_attr(attr.type()));
        }
        type_ptr = ctx->get_type(name, qualifiers);
    } else {
        HZCC_CHECK_OR_ASSIGN(base, utils::get_base_type(tokens, ctx));
        type_ptr = std::move(base);
    }
    // handle pointer(*)
    while (!tokens.empty() && tokens.peek().type() == TokenType::Mul) {
        tokens.pop();
        type_ptr = std::make_shared<ast::QualType>(
            std::make_shared<ast::PointerType>(type_ptr));
    }

    return std::make_unique<ast::TypeProxyExpr>(type.loc(), type_ptr);
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseCastExpr(TokenList& tokens,
                                                const SyntaxCtx& ctx) {
    if (StartsTypeName(tokens, *ctx)) {
        auto opening = tokens.pop();

        // type to_str
        HZCC_CHECK_OR_ASSIGN(type, ParseTypeName(tokens, ctx));
        const auto closing = tokens.empty() ? opening : tokens.peek();
        HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::RParentheses, tokens, opening));
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(closing, tokens));

        // cast expr
        HZCC_CHECK_OR_ASSIGN(expr, ParseCastExpr(tokens, ctx));

        return std::make_unique<ast::ExplicitCastExpr>(opening.loc(),  // NOLINT
                                                       std::move(expr),
                                                       std::move(type));
    }
    return ParseUnaryExpr(tokens, ctx);
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseUnaryExpr(TokenList& tokens,
                                                 const SyntaxCtx& ctx) {
    // some of unary expression does not have LHS
    if (tokens.peek().type() == TokenType::Sub ||
        tokens.peek().type() == TokenType::Reference ||
        tokens.peek().type() == TokenType::BitWiseNot ||
        tokens.peek().type() == TokenType::LogicalNot ||
        tokens.peek().type() == TokenType::Dereference) {
        auto op_type = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(op_type, tokens));
        HZCC_CHECK_OR_ASSIGN(rhs, ParseCastExpr(tokens, ctx));
        return std::make_unique<ast::UnaryOperator>(
            op_type.loc(), op_type.to_str(), std::move(rhs));
    }

    // cast expression
    else if (tokens.peek().type() == TokenType::Sizeof) {
        auto size_of = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(size_of, tokens));
        if (StartsTypeName(tokens, *ctx)) {
            auto opening = tokens.pop();
            HZCC_CHECK_OR_ASSIGN(type, ParseTypeName(tokens, ctx));
            HZCC_CHECK_OK_OR_RETURN(
                utils::Consume(TokenType::RParentheses, tokens, opening));
            return std::make_unique<ast::SizeofExpr>(size_of.loc(),
                                                     std::move(type));
        } else {
            HZCC_CHECK_OR_ASSIGN(expr, ParseUnaryExpr(tokens, ctx));
            return std::make_unique<ast::SizeofExpr>(size_of.loc(),
                                                     std::move(expr));
        }
    }

    // selfInc/selfDec and otherwise
    else if (tokens.peek().type() == TokenType::kSelfIncrement ||
             tokens.peek().type() == TokenType::kSelfDecrement) {
        // since self inc/dec could before the variable or after
        // the variable we need to check the next token

        auto type = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(type, tokens));

        // parsing RHS
        HZCC_CHECK_OR_ASSIGN(rhs, ParseUnaryExpr(tokens, ctx));

        if (!rhs->IsReturnLValue()) {
            return utils::TokenErr(type, "Expression is not assignable");
        }
        return std::make_unique<ast::UnaryOperator>(type.loc(), type.to_str(),
                                                    std::move(rhs));
    } else {
        return ParsePostfixExpr(tokens, ctx);
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParsePostfixExpr(TokenList& tokens,
                                                   const SyntaxCtx& ctx) {
    // first handle priority expression ("()")
    std::unique_ptr<ast::Expr> final_expr = nullptr;
    if (tokens.peek().type() == TokenType::LParentheses) {
        auto opening = tokens.pop();
        HZCC_CHECK_OK_OR_RETURN(RequireOperand(opening, tokens));
        HZCC_CHECK_OR_ASSIGN(expr, ParseCommaExpr(tokens, ctx));

        // consume the ')'
        HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::RParentheses, tokens, opening));

        final_expr = std::move(expr);
    } else {
        HZCC_CHECK_OR_ASSIGN(expr, ParseAccessExpr(ctx, tokens));
        final_expr = std::move(expr);
    }

    while (!tokens.empty()) {
        auto next = tokens.peek();

        // handle the case that have multiple [] after the ()
        if (next.type() == TokenType::LBracket) {
            tokens.pop();
            HZCC_CHECK_OK_OR_RETURN(RequireOperand(next, tokens));
            HZCC_CHECK_OR_ASSIGN(index, ParseCommaExpr(tokens, ctx));

            // consume the ']'
            HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::RBracket, tokens, next));
            final_expr = std::make_unique<ast::ArraySubscriptExpr>(
                next.loc(), std::move(final_expr), std::move(index));
        }

        // member access
        else if (next.type() == TokenType::kArrow || next.type() == TokenType::Dot) {
            // handle the case that have multiple . or -> after the ()
            tokens.pop();

            // accessor should not be a keyword
            if (tokens.empty() || tokens.peek().type() != TokenType::kIdentity) {
                return utils::TokenErr(
                    tokens.empty() ? next : tokens.peek(),
                    "Expect identifier\n        Expected a member name after '" +
                        next.to_str() + "'");
            }
            final_expr = std::make_unique<ast::MemberExpr>(
                next.type() == TokenType::kArrow, next.loc(), tokens.pop().to_str(),
                std::move(final_expr));
        }

        // post inc/dec
        else if (next.type() == TokenType::kSelfIncrement ||
                   next.type() == TokenType::kSelfDecrement) {
            // handle the case that have post inc/dec after the ()
            tokens.pop();
            if (!final_expr->IsReturnLValue()) {
                return utils::TokenErr(next, "Expression is not assignable");
            }
            final_expr = std::make_unique<ast::UnaryOperator>(next.loc(), next.to_str(),
                                                              std::move(final_expr));
        } else {
            break;
        }
    }
    return final_expr;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseAccessExpr(const SyntaxCtx& ctx,
                                                  TokenList& tokens) {
    // function caller
    if (tokens.peek().type() == TokenType::kIdentity) {
        // pop the identifier
        auto ident = tokens.pop();

        // function caller or variable
        if (!tokens.empty() && tokens.peek().type() == TokenType::LParentheses) {
            auto opening = tokens.pop();  // consume the '('

            // parse arguments
            std::list<std::unique_ptr<ast::Expr>> args_expr;
            while (!tokens.empty() && tokens.peek().type() != TokenType::RParentheses) {
                HZCC_CHECK_OR_ASSIGN(arg, ParseAssignExpr(tokens, ctx));
                args_expr.push_back(std::move(arg));
                if (tokens.empty() || tokens.peek().type() == TokenType::RParentheses) {
                    break;
                }

                // consume the ',' if have other arguments
                const auto comma = tokens.peek();
                HZCC_CheckAndConsume_ReturnErr(TokenType::Comma, tokens);

                // after consume the ',', we should have a
                // valid argument
                HZCC_CHECK_OK_OR_RETURN(RequireOperand(comma, tokens));
            }

            // consume the ')'
            HZCC_CHECK_OK_OR_RETURN(
                utils::Consume(TokenType::RParentheses, tokens, opening));
            auto call = std::make_unique<ast::FuncCallStmt>(ident.loc(), ident.to_str(),
                                                            std::move(args_expr));
            call->set_return_type(ctx->func_ret_type(ident.to_str()));
            return call;
        } else {
            // variable
            return std::make_unique<ast::DeclRefExpr>(ident.loc(),
                                                      ident.to_str());
        }
    }

    // const values (string literal will handle differently
    // since it could be concat)
    else if (tokens.peek().type() == TokenType::Char_Lit ||
             tokens.peek().type() == TokenType::Int_Lit ||
             tokens.peek().type() == TokenType::Real_Lit) {
        auto value = tokens.peek();

        switch (tokens.pop().type()) {
            case TokenType::Char_Lit:
                return std::make_unique<ast::LiteralExpr>(
                    LiteralType::Char, value.loc(), value.to_str());
            case TokenType::Int_Lit:
                return std::make_unique<ast::LiteralExpr>(
                    LiteralType::Integer, value.loc(), value.to_str());
            case TokenType::Real_Lit:
                return std::make_unique<ast::LiteralExpr>(
                    LiteralType::Real_number, value.loc(), value.to_str());
            default:
                LOG(FATAL) << "Unreachable code: \n"
                           << "\t Current ast:\n";
                return nullptr;
        }
    }

    // handle string literal
    else if (tokens.peek().type() == TokenType::Str_Lit) {
        auto lit_start = tokens.pop();
        auto value_str = lit_start.to_str();
        // concat all string literals
        while (!tokens.empty() && tokens.peek().type() == TokenType::Str_Lit) {
            value_str += tokens.pop().to_str();
        }
        return std::make_unique<ast::LiteralExpr>(LiteralType::String,
                                                  lit_start.loc(), value_str);
    }

    // if we can detect types
    else {
        if (tokens.peek().type() == TokenType::Else) {
            return syntax::utils::TokenErr(tokens,
                                           "Else has to be followed by if");
        } else {
            return syntax::utils::TokenErr(tokens, "term expected");
        }
    }
}
#pragma clang diagnostic pop

}  // namespace hzcc::syntax::parser
