//
// Created by chen_ on 2022/3/24.
//
#ifndef MYCC_SOURCE_SYNTAX_PARSER_COMMON_UTILS_H_
#define MYCC_SOURCE_SYNTAX_PARSER_COMMON_UTILS_H_
#include <list>
#include <memory>
#include <string>
#include "token_list_utils.h"
#include "lexical/utils/symbol_utils.h"

namespace Mycc {
namespace AST {
class Type;
class ASTNode;
class ASTContext;
}  // namespace AST
namespace Lexical {
class Token;
}  // namespace Lexical
namespace Syntax::Parser {

std::string TokenListToString(std::list<Lexical::Token>& tokens);

std::list<Lexical::Token> ParseTypeName(  // NOLINT
    AST::ASTContext& context,             // NOLINT
    std::list<Lexical::Token>& tokens);   // NOLINT

std::unique_ptr<AST::ASTNode> ParseCondition(  // NOLINT
    AST::ASTContext& context,                  // NOLINT
    std::list<Lexical::Token>& tokens);        // NOLINT

std::unique_ptr<AST::ASTNode> ParseBodyStatement(  // NOLINT
    AST::ASTContext& context,                      // NOLINT
    std::list<Lexical::Token>& tokens, bool add_semicolon = true);            // NOLINT

std::tuple<std::shared_ptr<AST::Type>, TokenList, Lexical::Token>
ParseTypeDecl(  // NOLINT
    std::string param,
    AST::ASTContext& context,            // NOLINT
    std::list<Lexical::Token>& tokens);  // NOLINT

std::tuple<Lexical::Token, std::list<std::unique_ptr<AST::ASTNode>>>
ParseVariable(                           // NOLINT
    AST::ASTContext& context,            // NOLINT
    std::list<Lexical::Token>& tokens);  // NOLINT

#define MYCC_CheckElse_MSG_ReturnNull(except, tokens, message)                 \
    if ((tokens).empty() || TokenListUtils::peek(tokens).Type() != (except)) { \
        MYCC_PrintFirstTokenError_ReturnNull(tokens, message);                 \
    } else

#define MYCC_CheckElse_ReturnNull(except, tokens) \
    MYCC_CheckElse_MSG_ReturnNull(                \
        except, tokens,                           \
        "Expected '" + Lexical::SymbolUtils::TokenTypeToString(except) + "'")

#define MYCC_CheckAndConsume_ReturnNull(except, tokens) \
    MYCC_CheckElse_ReturnNull(except, tokens) {         \
        TokenListUtils::pop_list(tokens);               \
    }

#define MYCC_CheckAndConsume_MSG_ReturnNull(except, tokens, message) \
    MYCC_CheckElse_MSG_ReturnNull(except, tokens, message)

}  // namespace Syntax::Parser
}  // namespace Mycc

#endif  // MYCC_SOURCE_SYNTAX_PARSER_COMMON_UTILS_H_
