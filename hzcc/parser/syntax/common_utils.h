//
// Created by chen_ on 2022/3/24.
//
#ifndef HZCC_PARSER_SYNTAX_UTILS_COMMON_UTILS_H_
#define HZCC_PARSER_SYNTAX_UTILS_COMMON_UTILS_H_
#include <absl/cleanup/cleanup.h>
#include <absl/strings/str_cat.h>
#include <iostream>
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
namespace syntax::utils {
/**
 * @brief Report a syntax diagnostic at one token and return its error status.
 * @param token Token supplying the source location.
 * @param message Diagnostic text.
 * @return Syntax-stage error status for the reported diagnostic.
 */
ALWAYS_INLINE Status TokenErr(const Token& token, std::string_view message) {
    std::cerr << "Parser error in file " << message::getFilename() << " line "
              << token.loc().first << " near text " << token.to_str(true) << '\n'
              << "        " << message << '\n';
    message::print_source_context(token.loc());
    return {StatusCode::kSyntaxStageErr, message};
}

/**
 * @brief Report a syntax diagnostic at the next token or at an unknown location.
 * @param token Token sequence; an empty sequence uses location (0, 0).
 * @param message Diagnostic text.
 * @return Syntax-stage error status; no token is consumed.
 */
ALWAYS_INLINE Status TokenErr(TokenList& token, std::string_view message) {
    if (token.empty()) {
        message::print_message(CompileErrorLevel::Error, message, {0, 0});
        return {StatusCode::kSyntaxStageErr, message};
    } else {
        return TokenErr(token.peek(), message);
    }
}

/**
 * @brief Consume a base type specification and apply cached qualifiers where supported.
 * @param tokens Input beginning at a primitive, record, or named type.
 * @param context Non-null compilation unit supplying and receiving named types.
 * @return Resolved qualified type, or a type lookup/syntax error.
 * @note Unsupported combinations return a syntax-stage diagnostic.
 */
ALWAYS_INLINE StatusOr<ast::QualTypePtr> get_base_type(
    TokenList& tokens, const std::shared_ptr<ast::CompilationUnit>& context) {
    if (tokens.empty()) return TokenErr(tokens, "Expected type name before end of input");
    auto first = tokens.pop();
    std::vector<Qualifier> qualifiers;
    for (const auto& attr : tokens.take_leading_attributes()) {
        qualifiers.push_back(to_attr(attr.type()));
    }
    if (first.type() == TokenType::Struct) {
        if (tokens.empty())
            return TokenErr(first, "Expected struct tag before end of input");
        // next token should be an identifier
        if (tokens.peek().type() != TokenType::kIdentity) {
            return TokenErr(tokens, "Expected identifier for struct tag");
        }
        auto tag = tokens.pop();
        auto name = "struct " + tag.to_str();
        if (context->has_type(name)) return context->get_type(name, qualifiers);
        return context->add_struct_type(tag.to_str(), qualifiers);
    }
    if (is_type_specifier(first.type()) && first.type() != TokenType::Auto) {
        // if this is a complex type
        if (!tokens.empty() && is_type_specifier(tokens.peek().type())) {
            // TODO: for complex type, like const long long int
            return TokenErr(
                tokens,
                absl::StrCat("Expected identifier; type combination '", first.to_str(),
                             " ", tokens.peek().to_str(), "' is not supported"));
        }

        // if this is a simple primitive type
        auto type = ast::GetNumericalTypeOf(first.type());
        return qualifiers.empty() ? type : type->type_of(qualifiers);
    }
    if (first.type() == TokenType::kIdentity) {
        if (context->has_type(first.to_str()))
            return context->get_type(first.to_str(), qualifiers);
        return TokenErr(
            first, absl::StrCat("Expected type name. Unknown type: ", first.to_str()));
    }
    if (first.type() == TokenType::Enum || first.type() == TokenType::Union) {
        return TokenErr(
            first, absl::StrCat(first.to_str(), " declarations are not supported yet"));
    }
    return TokenErr(first, "Expected type name");
}

/** @brief Consume the expected punctuation or report its first mismatch. */
inline Status Consume(TokenType expected, TokenList& tokens) {
    if (!tokens.empty() && tokens.peek().type() == expected) {
        tokens.pop();
        return NoError();
    }
    const auto wanted = Token(expected, 0, 0).to_str();
    if (tokens.empty()) {
        return TokenErr(tokens,
                        absl::StrCat("Expected '", wanted, "' before end of input"));
    }
    return TokenErr(tokens, absl::StrCat("Expected '", wanted, "', but got '",
                                         tokens.peek().to_str(true), "'"));
}

/** @brief Consume punctuation, anchoring an end-of-input error at its opening token. */
inline Status Consume(TokenType expected, TokenList& tokens, const Token& opening) {
    if (tokens.empty()) {
        return TokenErr(
            opening,
            absl::StrCat("Expected '", Token(expected, 0, 0).to_str(),
                         "' before end of input after '", opening.to_str(true), "'"));
    }
    return Consume(expected, tokens);
}

#define HZCC_CheckAndConsume_ReturnErr(EXPECTED, TOKENS) \
    HZCC_CHECK_OK_OR_RETURN(::hzcc::syntax::utils::Consume(EXPECTED, TOKENS))

/**
 * @brief Parse an expression enclosed in parentheses.
 * @param context Non-null compilation unit used by expression parsing.
 * @param tokens Input beginning at the opening parenthesis.
 * @return The condition expression, or a delimiter/expression parsing error.
 */
ALWAYS_INLINE StatusOr<ast::ExprPtr> ParseCondition(
    std::shared_ptr<ast::CompilationUnit>& context, TokenList& tokens) {
    if (tokens.empty())
        return TokenErr(tokens, "Expected '(' to start condition before end of input");
    // next token is (
    const auto opening = tokens.peek();
    HZCC_CheckAndConsume_ReturnErr(TokenType::LParentheses, tokens);
    if (tokens.empty())
        return TokenErr(opening, "Expected condition expression before end of input");

    HZCC_CHECK_OR_ASSIGN(condition,  // NOLINT
                         Parser::Parse<ast::Expr>(context, tokens));
    // next token is ')'
    HZCC_CHECK_OK_OR_RETURN(Consume(TokenType::RParentheses, tokens, opening));

    return condition;
}

/**
 * @brief Parse a braced body or wrap a single statement in a compound node.
 * @param context Non-null compilation unit whose scope is entered for a braced body.
 * @param tokens Input sequence beginning at the body statement.
 * @return Owned compound statement, or the first parsing error.
 * @note Block scope is restored on return.
 */
ALWAYS_INLINE StatusOr<std::unique_ptr<ast::CompoundStmt>> ParseBodyStatement(
    std::shared_ptr<ast::CompilationUnit>& context, TokenList& tokens) {
    if (tokens.empty())
        return TokenErr(tokens, "Expected statement body before end of input");
    if (tokens.peek().type() == TokenType::LBrace) {
        // enter new scope
        context->enter_scope();

        // leave scope
        absl::Cleanup leave_scope = [&] { context->leave_scope(); };
        return Parser::Parse<ast::CompoundStmt>(context, tokens);
    } else {
        HZCC_CHECK_OR_ASSIGN(body_statement, Parser::Parse<ast::Stmt>(context, tokens));

        // pack into a compound statement
        auto compound_stmt = std::make_unique<ast::CompoundStmt>(body_statement->loc());
        compound_stmt->add_stmt(std::move(body_statement));

        return compound_stmt;
    }
}

}  // namespace syntax::utils
// ax::utils
}  // namespace hzcc

#endif  // HZCC_PARSER_SYNTAX_UTILS_COMMON_UTILS_H_
