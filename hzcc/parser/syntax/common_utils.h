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
namespace syntax::utils {
ALWAYS_INLINE Status TokenErr(const Token& token, std::string_view message) {
    return CompileError(token.loc(),  // NOLINT
                        message);     // NOLINT
}

ALWAYS_INLINE Status TokenErr(TokenList& token, std::string_view message) {
    if (token.empty()) {
        message::print_message(CompileErrorLevel::Error, message, {0, 0});
        return {StatusCode::kSyntaxStageErr, message};
    } else {
        return TokenErr(token.peek(), message);
    }
}

ALWAYS_INLINE StatusOr<ast::QualTypePtr> get_base_type(
    TokenList& tokens, std::shared_ptr<ast::CompilationUnit>& context) {
    static std::vector<Qualifier> ret_attr_list(4);

    auto fist_token = tokens.pop();
    switch (fist_token.type()) {
        case TokenType::Enum:
        case TokenType::Union:
        case TokenType::Struct: {
            // next token should be an identifier
            if (tokens.peek().type() != TokenType::kIdentity) {
                return TokenErr(tokens.peek(), "except a identifier");
            } else {
                if (context->has_type("struct " + tokens.peek().to_str())) {
                    return TokenErr(tokens.peek(), "struct " + tokens.peek().to_str() +
                                                       " has been defined");
                } else {
                    ret_attr_list.clear();
                    auto base_type_list = tokens.cache_attr_list();
                    for (auto& attr : base_type_list) {
                        ret_attr_list.emplace_back(to_attr(attr.type()));
                    }
                    return context->add_struct_type(tokens.pop().to_str(), ret_attr_list);
                }
            }
        } break;

        PRIMITIVE_TYPE_CASES : {
            // if this is a simple primitive type
            if (!is_type_specifier(tokens.peek().type())) {
                return ast::GetNumericalTypeOf(fist_token.type());
            }

            // if this is a complex type
            else {
                // complex type will have at most 4 specifier
                static std::vector<TokenType> ret_attr_list(4);

                // collect all the specifier
                ret_attr_list.clear();
                ret_attr_list.emplace_back(fist_token.type());
                while (is_type_specifier(tokens.peek().type())) {
                    ret_attr_list.emplace_back(tokens.pop().type());
                }

                // TODO: for complex type, like const long long int
                LOG(FATAL) << "Complex type is not supported yet";
            }
        } break;

        case TokenType::kIdentity: {
            if (context->has_type(fist_token.to_str())) {
                ret_attr_list.clear();
                auto base_type_list = tokens.cache_attr_list();
                for (auto& attr : base_type_list) {
                    ret_attr_list.emplace_back(to_attr(attr.type()));
                }
                return context->get_type(fist_token.to_str(), ret_attr_list);
            } else {
                return hzcc::syntax::utils::TokenErr(
                    fist_token, "Unknown type: " + fist_token.to_str());
            }
        } break;
        default: {
            return TokenErr(fist_token, "except a type");
        }
    }
}

#define HZCC_CheckAndConsume_ReturnErr(EXCEPT, TOKENS)                                  \
    static_assert(std::is_same_v<hzcc::TokenType, decltype(EXCEPT)>,                    \
                  "Macro type mismatch, need lexical::Token for 1st variable");         \
    static_assert(                                                                      \
        std::disjunction_v<std::is_same<hzcc::TokenList&, decltype(TOKENS)>,            \
                           std::is_same<const hzcc::TokenList&, decltype(TOKENS)>>,     \
        "Macro type mismatch, need syntax::TokenList& for 2nd variable");               \
    if ((TOKENS).empty() || (TOKENS).peek().type() != (EXCEPT)) {                       \
        return syntax::utils::TokenErr(                                                 \
            TOKENS,                                                                     \
            absl::StrCat("Expected '", magic_enum::enum_name((EXCEPT)), "', but got '", \
                         magic_enum::enum_name((TOKENS).peek().type()), "'"));          \
    } else {                                                                            \
        (TOKENS).pop();                                                                 \
    };

ALWAYS_INLINE StatusOr<ast::ExprPtr> ParseCondition(
    std::shared_ptr<ast::CompilationUnit>& context, TokenList& tokens) {
    // next token is (
    HZCC_CheckAndConsume_ReturnErr(TokenType::LParentheses, tokens);

    HZCC_CHECK_OR_ASSIGN(condition,  // NOLINT
                         Parser::Parse<ast::Expr>(context, tokens))
    // next token is ')'
    HZCC_CheckAndConsume_ReturnErr(TokenType::RParentheses, tokens);

    return condition;
}

ALWAYS_INLINE StatusOr<std::unique_ptr<ast::CompoundStmt>> ParseBodyStatement(
    std::shared_ptr<ast::CompilationUnit>& context, bool add_semicolon,
    TokenList& tokens) {
    if (tokens.peek().type() == TokenType::LBrace) {
        // enter new scope
        context->enter_scope();

        HZCC_CHECK_OR_ASSIGN(body_statement,  // NOLINT
                             Parser::Parse<ast::CompoundStmt>(context, tokens))
        // leave scope
        context->leave_scope();

        if (add_semicolon) {
            // push a semicolon for easier parsing
            tokens.push(TokenType::SemiColon, -1, -1);
        }

        return body_statement;
    } else {
        HZCC_CHECK_OR_ASSIGN(body_statement, Parser::Parse<ast::Stmt>(context, tokens))

        if (!body_statement->has_body()) {
            // consume ';'
            HZCC_CheckAndConsume_ReturnErr(TokenType::SemiColon, tokens)
        }

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
