//
// Created by chen_ on 2022/3/24.
//
#ifndef MYCC_SOURCE_SYNTAX_PARSER_COMMON_UTILS_H_
#define MYCC_SOURCE_SYNTAX_PARSER_COMMON_UTILS_H_
#include <list>
#include <memory>
#include <string>

#include "lexical/utils/symbol_utils.h"
#include "syntax/SyntaxContext.h"
#include "syntax/TokenList.h"
namespace Hzcc {
namespace AST {
class Type;
class ASTNode;
class CompilationUnit;
}  // namespace AST
namespace Lexical {
class Token;
}  // namespace Lexical
namespace Syntax::Parser {

std::string TokenListToString(std::list<Lexical::Token>& tokens);

std::list<Lexical::Token> ParseTypeName(TokenList& tokens,
                                        SyntaxContext& context);

std::unique_ptr<AST::ASTNode> ParseCondition(TokenList& tokens,
                                             SyntaxContext& context);

std::unique_ptr<AST::ASTNode> ParseBodyStatement(TokenList& tokens,
                                                 SyntaxContext& context,
                                                 bool add_semicolon);

std::tuple<std::shared_ptr<AST::Type>, Lexical::Token> ParseTypeDecl(
    const std::string& param, TokenList& tokens, SyntaxContext& context);

std::tuple<Lexical::Token, std::list<std::unique_ptr<AST::ASTNode>>>
ParseVariable(SyntaxContext& context, TokenList& tokens);

#define MYCC_CheckAndConsume_ReturnNull(except, tokens)                       \
    {                                                                         \
        static_assert(                                                        \
            std::is_same_v<Lexical::TokenType, decltype(except)>,             \
            "Macro type mismatch, need Lexical::Token for 1st variable");     \
        static_assert(                                                        \
            std::is_same_v<Syntax::TokenList&, decltype(tokens)> ||           \
                std::is_same_v<const Syntax::TokenList&, decltype(tokens)>,   \
            "Macro type mismatch, need Syntax::TokenList& for 2nd variable"); \
        if ((tokens).empty() || (tokens).peek().Type() != (except)) {         \
            MYCC_PrintFirstTokenError_ReturnNull(                             \
                tokens, "Expected '" +                                        \
                            Lexical::SymbolUtils::TokenTypeToString(except) + \
                            "', but got '" +                                  \
                            Lexical::SymbolUtils::TokenTypeToString(          \
                                (tokens).peek().Type()) +                     \
                            "'");                                             \
        } else {                                                              \
            (tokens).pop();                                                   \
        }                                                                     \
    }

#define MYCC_PrintTokenError(token, message)                               \
                                                                           \
    {                                                                      \
        static_assert(                                                     \
            std::is_same_v<Lexical::Token, decltype(token)> ||             \
                std::is_same_v<Lexical::Token&, decltype(token)> ||        \
                std::is_same_v<const Lexical::Token, decltype(token)> ||   \
                std::is_same_v<const Lexical::Token&, decltype(token)>,    \
            "Macro type mismatch, need Lexical::Token& for 1st variable"); \
        Message::print_message(Message::kError, (message),                 \
                               (token).SourceLine(), (token).Location(),   \
                               (token).Value());                           \
    }

#define MYCC_PrintFirstTokenError(tokens, message)                          \
    {                                                                       \
        static_assert(                                                      \
            std::is_same_v<Syntax::TokenList&, decltype(tokens)> ||         \
                std::is_same_v<const Syntax::TokenList&, decltype(tokens)>, \
            "Macro type mismatch,  const Syntax::TokenList& for 1st "       \
            "variable");                                                    \
        const auto& error_token = (tokens).peek();                          \
        MYCC_PrintTokenError(error_token, message)                          \
    }

#define MYCC_PrintTokenError_ReturnNull(token, message) \
    MYCC_PrintTokenError(token, message) return nullptr;

#define MYCC_PrintFirstTokenError_ReturnNull(tokens, message)                \
    {                                                                        \
        static_assert(                                                       \
            std::is_same_v<Syntax::TokenList, decltype(tokens)> ||           \
                std::is_same_v<Syntax::TokenList&, decltype(tokens)> ||      \
                std::is_same_v<const Syntax::TokenList, decltype(tokens)> || \
                std::is_same_v<const Syntax::TokenList&, decltype(tokens)>,  \
            "Macro type mismatch, need  Syntax::TokenList& for 1st "         \
            "variable");                                                     \
        const auto& error_token =                                            \
            (tokens).empty()                                                 \
                ? Lexical::Token(Lexical::TokenType::kUnknown, -1, -1)       \
                : (tokens).peek();                                           \
        MYCC_PrintTokenError_ReturnNull(error_token, message)                \
    }  // namespace Syntax::Parser

}  // namespace Syntax::Parser
}  // namespace Hzcc

#endif  // MYCC_SOURCE_SYNTAX_PARSER_COMMON_UTILS_H_
