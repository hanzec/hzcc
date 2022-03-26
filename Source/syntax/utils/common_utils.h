//
// Created by chen_ on 2022/3/24.
//
#include <list>
#include <memory>

#include "lexical/token_type.h"
#include "lexical/utils/symbol_utils.h"
#include "macro.h"
#include "utils/message_utils.h"
#ifndef MYCC_SOURCE_SYNTAX_PARSER_COMMON_UTILS_H_
#define MYCC_SOURCE_SYNTAX_PARSER_COMMON_UTILS_H_
namespace Mycc {
namespace AST {
class ASTNode;
class ASTContext;
}  // namespace AST
namespace Lexical {
class Token;
}  // namespace Lexical
namespace Syntax::Parser {

std::unique_ptr<AST::ASTNode> ParseCondition(  // NOLINT
    AST::ASTContext& context,                  // NOLINT
    std::list<Lexical::Token> tokens);         // NOLINT

std::unique_ptr<AST::ASTNode> ParseBodyStatement(  // NOLINT
    AST::ASTContext& context,                      // NOLINT
    std::list<Lexical::Token> tokens);             // NOLINT

#define MYCC_CheckElse_ReturnNull(except, tokens)                             \
    if (TokenListUtils::peek(tokens).Type() != (except)) {                    \
        MYCC_PrintFirstTokenError_ReturnNull(                                 \
            tokens, "Except an " +                                            \
                        Lexical::SymbolUtils::TokenTypeToString(except) +     \
                        " but got: " +                                        \
                        ((int)error_token.Type() <= 0xFF                      \
                             ? "[Symbol"                                      \
                             : "[" + Lexical::SymbolUtils::TokenTypeToString( \
                                         error_token.Type())) +               \
                        "]:@" + error_token.Value() + "@");                   \
    } else

#define MYCC_CheckAndConsume_ReturnNull(except, tokens) \
    MYCC_CheckElse_ReturnNull(except, tokens) { (tokens).pop_front(); }

}  // namespace Syntax::Parser
}  // namespace Mycc

#endif  // MYCC_SOURCE_SYNTAX_PARSER_COMMON_UTILS_H_
