//
// Created by chen_ on 2022/3/24.
//
#include <glog/logging.h>

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
Statement::Statement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::Stmt>(),
                 TypeNameUtil::name_pretty<ast::Stmt>()) {}

StatusOr<ast::StmtPtr> Statement::parse_impl(SyntaxCtx ctx, TokenList& tokens) {
    switch (tokens.peek().type()) {
        case TokenType::SemiColon:
            return std::make_unique<ast::EmptyStmt>();
        case TokenType::Struct:
            if (tokens.peek3().type() == TokenType::LBrace) {
                return Parser::Parse<ast::DeclStmt>(ctx, tokens);
            } else {
                return ParseCommaExpr(tokens, ctx);
            }
        case TokenType::Return: {
            auto token = tokens.pop();
            if (tokens.peek().type() != TokenType::SemiColon) {
                HZCC_CHECK_OR_ASSIGN(ret_expr, ParseCommaExpr(tokens, ctx))
                return std::make_unique<ast::ReturnStmt>(token.loc(),
                                                         std::move(ret_expr));
            } else {
                DVLOG(SYNTAX_LOG) << "ignore ';' since return stmt is empty";
                return std::make_unique<ast::ReturnStmt>(
                    token.loc(), std::make_unique<ast::EmptyExpr>());
            }
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
            if (WithinLoop()) {
                tokens.pop();
                return std::make_unique<ast::BreakStmt>(tokens.peek().loc());
            } else {
                return syntax::utils::TokenErr(tokens,
                                               "break stmt not within a loop");
            }
        case TokenType::Continue:
            if (WithinLoop()) {
                tokens.pop();
                return std::make_unique<ast::ContinueStmt>(tokens.peek().loc());
            } else {
                return syntax::utils::TokenErr(
                    tokens, "continue stmt not within a loop");
            }
        case TokenType::Int:
        case TokenType::Char:
        case TokenType::Float:
        case TokenType::Double:
        case TokenType::Void:
        case TokenType::Long:
        case TokenType::Short:
        case TokenType::Bool:
        case TokenType::Complex:
        case TokenType::Imaginary:
        case TokenType::Signed:
        case TokenType::Unsigned:
        case TokenType::Auto:
        case TokenType::kIdentity: {
            return Parser::Parse<ast::VarDecl>(ctx, tokens);
        }
        default:
            return ParseCommaExpr(tokens, ctx);
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseCommaExpr(TokenList& tokens,
                                                 const SyntaxCtx& context) {
    // parsing left-hand side of expression
    HZCC_CHECK_OR_ASSIGN(lhs, ParseAssignExpr(tokens, context))

    // if we have a comma, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().type() == TokenType::Comma) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseCommaExpr(tokens, context))
        return std::make_unique<ast::CommaExpr>(next.loc(), std::move(lhs),
                                                std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseAssignExpr(TokenList& tokens,
                                                  const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseConditionalExpr(tokens, ctx))

    // if we have a assign cast, we should parse the right-hand side
    if (tokens.peek().type() == TokenType::Assign ||
        tokens.peek().type() == TokenType::kAddAssign ||
        tokens.peek().type() == TokenType::kSubAssign ||
        tokens.peek().type() == TokenType::kMulAssign ||
        tokens.peek().type() == TokenType::kDivAssign ||
        tokens.peek().type() == TokenType::kModAssign ||
        tokens.peek().type() == TokenType::kLShiftAssign ||
        tokens.peek().type() == TokenType::kRShiftAssign ||
        tokens.peek().type() == TokenType::kAndAssign ||
        tokens.peek().type() == TokenType::kXorAssign ||
        tokens.peek().type() == TokenType::kOrAssign) {
        auto assign_type = tokens.peek();
        tokens.pop();

        HZCC_CHECK_OR_ASSIGN(rhs, ParseAssignExpr(tokens, ctx))
        return std::make_unique<ast::AssignExpr>(
            assign_type.loc(), assign_type.to_str(), std::move(lhs),
            std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

// TODO: add syntax for conditional expression
#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseConditionalExpr(TokenList& tokens,
                                                       const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseLogicalOrExpr(tokens, ctx))

    if (tokens.peek().type() == TokenType::QuestionMark) {
        auto cond_loc = tokens.peek().loc();
        tokens.pop();  // pop '?'

        // parse true expression
        HZCC_CHECK_OR_ASSIGN(true_expr, ParseConditionalExpr(tokens, ctx))

        // pop ':'
        HZCC_CheckAndConsume_ReturnErr(TokenType::Colon, tokens);

        // parse false expression
        HZCC_CHECK_OR_ASSIGN(false_expr, ParseConditionalExpr(tokens, ctx))
        return std::make_unique<ast::TernaryExpr>(cond_loc, std::move(lhs),
                                                  std::move(true_expr),
                                                  std::move(false_expr));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseLogicalOrExpr(TokenList& tokens,
                                                     const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseLogicalAndExpr(tokens, ctx))

    // if we have a logical or, we should parse the next expression
    if (tokens.peek().type() == TokenType::kLogicalOr) {
        auto op = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseLogicalOrExpr(tokens, ctx))
        return std::make_unique<ast::LogicalExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseLogicalAndExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseBitwiseOrExpr(tokens, ctx))

    // if we have a logical and, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().type() == TokenType::kLogicalAnd) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseLogicalAndExpr(tokens, ctx))
        return std::make_unique<ast::LogicalExpr>(
            next.loc(), next.to_str(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseBitwiseOrExpr(TokenList& tokens,
                                                     const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseBitwiseXorExpr(tokens, ctx))

    // if we have a bitwise or, we should parse the next expression
    if (tokens.peek().type() == TokenType::BitwiseOr) {
        auto op = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseBitwiseOrExpr(tokens, ctx))

        return std::make_unique<ast::BitwiseExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseBitwiseXorExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseBitwiseAndExpr(tokens, ctx))

    // if we have a bitwise xor, we should parse the next expression
    if (tokens.peek().type() == TokenType::BitwiseXor) {
        auto op = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseBitwiseXorExpr(tokens, ctx))
        return std::make_unique<ast::BitwiseExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseBitwiseAndExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseEqualityExpr(tokens, ctx))

    if (tokens.peek().type() == TokenType::BitwiseAnd) {
        auto op = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseBitwiseAndExpr(tokens, ctx))
        return std::make_unique<ast::BitwiseExpr>(
            op.loc(), op.to_str(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseEqualityExpr(TokenList& tokens,
                                                    const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseRelationalExpr(tokens, ctx))

    if (tokens.peek().type() == TokenType::EQ ||
        tokens.peek().type() == TokenType::NE) {
        auto op_type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseEqualityExpr(tokens, ctx))
        return std::make_unique<ast::RelationalExpr>(
            op_type.loc(), op_type.to_str(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseRelationalExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseShiftExpr(tokens, ctx))

    if (tokens.peek().type() == TokenType::Less ||
        tokens.peek().type() == TokenType::Greater ||
        tokens.peek().type() == TokenType::LE ||
        tokens.peek().type() == TokenType::GE) {
        auto op_type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseRelationalExpr(tokens, ctx))
        return std::make_unique<ast::RelationalExpr>(
            op_type.loc(), op_type.to_str(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseShiftExpr(TokenList& tokens,
                                                 const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseAdditiveExpr(tokens, ctx))

    // if we have a shift, we should parse the next expression
    if (tokens.peek().type() == TokenType::kLeftShift ||
        tokens.peek().type() == TokenType::kRightShift) {
        auto op_type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseShiftExpr(tokens, ctx))
        return std::make_unique<ast::BitwiseExpr>(
            op_type.loc(), op_type.to_str(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseAdditiveExpr(TokenList& tokens,
                                                    const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseMultiplicativeExpr(tokens, ctx))

    // if we have a additive, we should parse the next
    // expression
    if (tokens.peek().type() == TokenType::Add ||
        tokens.peek().type() == TokenType::Sub) {
        auto type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseAdditiveExpr(tokens, ctx))
        return std::make_unique<ast::ArithmeticExpr>(
            type.loc(), type.to_str(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::ExprPtr> Statement::ParseMultiplicativeExpr(
    TokenList& tokens, const SyntaxCtx& ctx) {
    HZCC_CHECK_OR_ASSIGN(lhs, ParseUnaryExpr(tokens, ctx))

    // if we have a multiplicative, we should parse the next
    // expression
    if (tokens.peek().type() == TokenType::Mul ||
        tokens.peek().type() == TokenType::Div ||
        tokens.peek().type() == TokenType::Mod) {
        auto type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseMultiplicativeExpr(tokens, ctx))
        return std::make_unique<ast::ArithmeticExpr>(
            type.loc(), type.to_str(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::TypeProxyExprPtr> Statement::ParseTypeName(TokenList& tokens,
                                                         const SyntaxCtx& ctx) {
    auto type = tokens.pop();

    // handle base types
    ast::QualTypePtr type_ptr;
    if (parser_common::is_primitive_type(type.to_str().c_str())) {
        type_ptr = ast::GetNumericalTypeOf(to_primitive_type(type.to_str()));
    } else {
        if (type.type() == TokenType::Enum ||
            type.type() == TokenType::Struct) {
            auto type_name = tokens.pop();

            if (ctx->has_type(type.to_str() + " " + type_name.to_str())) {
                type_ptr =
                    ctx->get_type(type.to_str() + " " + type_name.to_str());
            } else {
                return CompileError(type.loc(), "unknown type");
            }
        } else {
            if (ctx->has_type(type.to_str())) {
                type_ptr = ctx->get_type(type.to_str());
            } else {
                return CompileError(type.loc(), "unknown type");
            }
        }
    }

    // handle pointer(*)
    while (tokens.peek().type() == TokenType::Mul) {
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
    if (tokens.peek().type() == TokenType::RParentheses ||
        ctx->has_type(tokens.peek2().to_str())) {
        auto loc = tokens.peek().loc();
        tokens.pop();

        // type to_str
        HZCC_CHECK_OR_ASSIGN(type, ParseTypeName(tokens, ctx))
        HZCC_CheckAndConsume_ReturnErr(TokenType::RParentheses, tokens);

        // cast expr
        HZCC_CHECK_OR_ASSIGN(expr, ParseCastExpr(tokens, ctx))

        return std::make_unique<ast::ExplicitCastExpr>(loc,  // NOLINT
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
        auto op_type = tokens.peek();
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(rhs, ParseCastExpr(tokens, ctx))
        return std::make_unique<ast::UnaryOperator>(
            op_type.loc(), op_type.to_str(), std::move(rhs));
    }

    // cast expression
    else if (tokens.peek().type() == TokenType::Sizeof) {
        auto size_of = tokens.pop();
        if (tokens.peek().type() == TokenType::LParentheses &&
            (tokens.peek().type() == TokenType::Struct ||
             ctx->has_type(tokens.peek2().to_str()))) {
            tokens.pop();
            HZCC_CHECK_OR_ASSIGN(type, ParseTypeName(tokens, ctx))
            HZCC_CheckAndConsume_ReturnErr(TokenType::RParentheses, tokens);
            return std::make_unique<ast::SizeofExpr>(size_of.loc(),
                                                     std::move(type));
        } else {
            HZCC_CHECK_OR_ASSIGN(expr, ParseUnaryExpr(tokens, ctx))
            return std::make_unique<ast::SizeofExpr>(size_of.loc(),
                                                     std::move(expr));
        }
    }

    // selfInc/selfDec and otherwise
    else if (tokens.peek().type() == TokenType::kSelfIncrement ||
             tokens.peek().type() == TokenType::kSelfDecrement) {
        // since self inc/dec could before the variable or after
        // the variable we need to check the next token

        auto type = tokens.peek();
        tokens.pop();

        // parsing RHS
        HZCC_CHECK_OR_ASSIGN(rhs, ParseUnaryExpr(tokens, ctx))

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
        tokens.pop();  // consume the '('

        HZCC_CHECK_OR_ASSIGN(expr, ParseCommaExpr(tokens, ctx))

        // consume the ')'
        HZCC_CheckAndConsume_ReturnErr(TokenType::RParentheses, tokens);

        final_expr = std::move(expr);
    } else {
        HZCC_CHECK_OR_ASSIGN(expr, ParseAccessExpr(ctx, tokens))
        final_expr = std::move(expr);
    }

    // should never be null
    HZCC_RUNTIME_CHECK_BLOCK({
        LOG_IF(FATAL, final_expr == nullptr)
            << "final_expr should not be nullptr";
    })

    if (tokens.peek().type() == TokenType::LBracket) {
        // handle the case that have multiple [] after the ()
        while (tokens.peek().type() == TokenType::LBracket) {
            auto loc = tokens.pop().loc();  // consume the '['

            HZCC_CHECK_OR_ASSIGN(expr, ParseCommaExpr(tokens, ctx))

            // consume the ']'
            HZCC_CheckAndConsume_ReturnErr(TokenType::RBracket, tokens);

            final_expr = std::make_unique<ast::ArraySubscriptExpr>(
                loc, std::move(final_expr), std::move(expr));
        }
        return std::move(final_expr);
    }

    // member access
    else if (tokens.peek().type() == TokenType::kArrow ||
             tokens.peek().type() == TokenType::Dot) {
        // handle the case that have multiple . or -> after the ()
        while (tokens.peek().type() == TokenType::kArrow ||
               tokens.peek().type() == TokenType::Dot) {
            auto loc = tokens.pop().loc();  // consume the '.' or '->'
            bool isPtr = tokens.pop().type() == TokenType::kArrow;

            // accessor should not be a keyword
            if (tokens.peek().type() != TokenType::kIdentity) {
                return syntax::utils::TokenErr(tokens, "Expect identifier");
            }

            final_expr = std::make_unique<ast::MemberExpr>(
                isPtr, loc, tokens.pop().to_str(), std::move(final_expr));
        }
        return std::move(final_expr);
    }

    // post inc/dec
    if (tokens.peek().type() == TokenType::kSelfIncrement ||
        tokens.peek().type() == TokenType::kSelfDecrement) {
        // handle the case that have post inc/dec after the ()
        while (tokens.peek().type() == TokenType::kSelfIncrement ||
               tokens.peek().type() == TokenType::kSelfDecrement) {
            auto type = tokens.pop();

            final_expr = std::make_unique<ast::UnaryOperator>(
                type.loc(), type.to_str(), std::move(final_expr));
        }
        return std::move(final_expr);
    } else {
        return std::move(final_expr);
    }
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
        if (tokens.peek().type() == TokenType::LParentheses) {
            tokens.pop();  // consume the '('

            // parse arguments
            std::list<std::unique_ptr<ast::Expr>> args_expr;
            while (tokens.peek().type() != TokenType::RParentheses) {
                HZCC_CHECK_OR_ASSIGN(arg, ParseAssignExpr(tokens, ctx))

                // consume the ',' if have other arguments
                if (tokens.peek().type() != TokenType::RParentheses) {
                    HZCC_CheckAndConsume_ReturnErr(TokenType::Comma, tokens);

                    // after consume the ',', we should have a
                    // valid argument
                    if (tokens.peek().type() == TokenType::RParentheses) {
                        return syntax::utils::TokenErr(tokens.peek(),
                                                       "term expected");
                    }
                }
                args_expr.push_back(std::move(arg));
            }

            // consume the ')'
            HZCC_CheckAndConsume_ReturnErr(TokenType::RParentheses, tokens);

            return std::make_unique<ast::FuncCallStmt>(
                ident.loc(), ident.to_str(), std::move(args_expr));
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
        while (tokens.peek().type() == TokenType::Str_Lit) {
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
