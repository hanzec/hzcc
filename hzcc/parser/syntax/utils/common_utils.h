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
#include "parser/syntax/Ctx.h"
namespace hzcc {
namespace ast {
class Type;
class Stmt;
class CompilationUnit;
}  // namespace ast
using TypeNamePair = std::pair<TokenType, Token>;

std::string ALWAYS_INLINE to_string(const TypeNamePair& tokens) {
    return std::string(to_string(tokens.first)) + " " + tokens.second.Value();
}

namespace syntax::utils {
using VariableList = std::tuple<std::shared_ptr<ast::Type>, hzcc::Token>;

std::list<hzcc::ast::Attribute> ConvertAttributeList(
    std::list<hzcc::Token>& tokens);

StatusOr<ast::ExprPtr> ParseCondition(TokenList& tokens,
                                      hzcc::syntax::Ctx& context);

StatusOr<ast::StmtPtr> ParseBodyStatement(TokenList& tokens,
                                          hzcc::syntax::Ctx& context,
                                          bool add_semicolon);

std::tuple<std::shared_ptr<ast::Type>, hzcc::Token> ParseTypeDecl(
    const std::string& param, TokenList& tokens, hzcc::syntax::Ctx& context);

VariableList ParseVariable(hzcc::syntax::Ctx& context, TokenList& tokens);

ALWAYS_INLINE Status TokenErr(const hzcc::Token& token,
                              const std::string& message) {
    message::print_message(kCompileErrorLevel_Error, message, token.Location());
    return {StatusCode::kSyntaxStageErr, message};
}

ALWAYS_INLINE Status TokenErr(TokenList& token, const std::string& message) {
    if (token.empty()) {
        message::print_message(kCompileErrorLevel_Error, message, {0, 0});
        return {StatusCode::kSyntaxStageErr, message};
    } else {
        return TokenErr(token.peek(), message);
    }
}

#define HZCC_CheckAndConsume_ReturnErr(EXCEPT, TOKENS)                        \
    {                                                                         \
        static_assert(                                                        \
            std::is_same_v<hzcc::TokenType, decltype(EXCEPT)>,                \
            "Macro type mismatch, need lexical::Token for 1st variable");     \
        static_assert(                                                        \
            std::disjunction_v<                                               \
                std::is_same<hzcc::TokenList&, decltype(TOKENS)>,             \
                std::is_same<const hzcc::TokenList&, decltype(TOKENS)>>,      \
            "Macro type mismatch, need syntax::TokenList& for 2nd variable"); \
        if ((TOKENS).empty() || (TOKENS).peek().Type() != (EXCEPT)) {         \
            return syntax::utils::TokenErr(                                   \
                TOKENS, "Expected '" + hzcc::to_string(EXCEPT) +              \
                            "', but got '" +                                  \
                            hzcc::to_string((TOKENS).peek().Type()) + "'");   \
        } else {                                                              \
            (TOKENS).pop();                                                   \
        }                                                                     \
    }
}  // namespace syntax::utils
}  // namespace hzcc

#endif  // HZCC_PARSER_SYNTAX_UTILS_COMMON_UTILS_H_
