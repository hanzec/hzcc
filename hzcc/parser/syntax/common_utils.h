//
// Created by chen_ on 2022/3/24.
//
#ifndef HZCC_PARSER_SYNTAX_UTILS_COMMON_UTILS_H_
#define HZCC_PARSER_SYNTAX_UTILS_COMMON_UTILS_H_
#include <list>
#include <memory>
#include <string>

#include "parser/common/Token.h"
#include "parser/common/keywords.h"
#include "parser/common/token_type.h"
#include "parser/parser.h"
#include "parser/syntax/parser/parser_factory.h"
namespace hzcc {
namespace ast {
class Type;
class Stmt;
class CompilationUnit;
}  // namespace ast
using TypeNamePair = std::pair<TokenType, Token>;

std::string ALWAYS_INLINE to_string(const TypeNamePair& tokens) {
    return std::string(to_string(tokens.first)) + " " + tokens.second.val();
}

namespace syntax::utils {
ALWAYS_INLINE Status TokenErr(const Token& token, const std::string& message) {
    return CompileError(token.loc(),  // NOLINT
                        message);     // NOLINT
}

ALWAYS_INLINE Status TokenErr(TokenList& token, const std::string& message) {
    if (token.empty()) {
        message::print_message(kCompileErrorLevel_Error, message, {0, 0});
        return {StatusCode::kSyntaxStageErr, message};
    } else {
        return TokenErr(token.peek(), message);
    }
}

ALWAYS_INLINE StatusOr<TypeNamePair> get_base_type(
    TokenList& tokens, std::shared_ptr<ast::CompilationUnit>& context) {
    auto fist_token = tokens.pop();
    if (fist_token.Type() == TokenType::kEnum ||
        fist_token.Type() == TokenType::kUnion ||
        fist_token.Type() == TokenType::kStruct) {
        // next token should be an identifier
        if (tokens.peek().Type() != TokenType::kIdentity) {
            return hzcc::syntax::utils::TokenErr(tokens.peek(),
                                                 "except a identifier");
        } else {
            if (context->has_type("struct " + tokens.peek().val())) {
                return hzcc::syntax::utils::TokenErr(
                    tokens.peek(),
                    "struct " + tokens.peek().val() + " has been defined");
            } else {
                return std::make_pair(fist_token.Type(), tokens.pop());
            }
        }
    } else {
        if (context->has_type(fist_token.val())) {
            return std::make_pair(fist_token.Type(), fist_token);
        } else {
            return hzcc::syntax::utils::TokenErr(
                fist_token,
                "type " + fist_token.val() + " has not been defined");
        }
    }
}

#define HZCC_CheckAndConsume_ReturnErr(EXCEPT, TOKENS)                        \
    static_assert(                                                            \
        std::is_same_v<hzcc::TokenType, decltype(EXCEPT)>,                    \
        "Macro type mismatch, need lexical::Token for 1st variable");         \
    static_assert(                                                            \
        std::disjunction_v<                                                   \
            std::is_same<hzcc::TokenList&, decltype(TOKENS)>,                 \
            std::is_same<const hzcc::TokenList&, decltype(TOKENS)>>,          \
        "Macro type mismatch, need syntax::TokenList& for 2nd variable");     \
    if ((TOKENS).empty() || (TOKENS).peek().Type() != (EXCEPT)) {             \
        return syntax::utils::TokenErr(                                       \
            TOKENS, "Expected '" + hzcc::to_string(EXCEPT) + "', but got '" + \
                        hzcc::to_string((TOKENS).peek().Type()) + "'");       \
    } else {                                                                  \
        (TOKENS).pop();                                                       \
    };

ALWAYS_INLINE StatusOr<ast::ExprPtr> ParseCondition(
    std::shared_ptr<ast::CompilationUnit>& context, TokenList& tokens) {
    // next token is (
    HZCC_CheckAndConsume_ReturnErr(TokenType::kLParentheses, tokens);

    HZCC_CHECK_OR_ASSIGN(condition,  // NOLINT
                         Parser::Parse<ast::Expr>(context, tokens))
    // next token is ')'
    HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens);

    return condition;
}

ALWAYS_INLINE StatusOr<ast::StmtPtr> ParseBodyStatement(
    std::shared_ptr<ast::CompilationUnit>& context, bool add_semicolon,
    TokenList& tokens) {
    if (tokens.peek().Type() == TokenType::kLBrace) {
        // enter new scope
        context->enter_scope();

        HZCC_CHECK_OR_ASSIGN(body_statement,  // NOLINT
                             Parser::Parse<ast::CompoundStmt>(context, tokens))
        // leave scope
        context->leave_scope();

        if (add_semicolon) {
            // push a semicolon for easier parsing
            tokens.push(TokenType::kSemiColon, -1, -1);
        }

        return body_statement;
    } else {
        HZCC_CHECK_OR_ASSIGN(body_statement,
                             Parser::Parse<ast::Stmt>(context, tokens))

        if (!body_statement->has_body()) {
            // consume ';'
            HZCC_CheckAndConsume_ReturnErr(TokenType::kSemiColon, tokens)
        }

        return body_statement;
    }
}

}  // namespace syntax::utils
// ax::utils
}  // namespace hzcc

#endif  // HZCC_PARSER_SYNTAX_UTILS_COMMON_UTILS_H_
