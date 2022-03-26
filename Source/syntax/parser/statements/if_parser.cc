//
// Created by chen_ on 2022/3/24.
//
#include "AST/statement/if_parser.h"

#include <list>

#include "AST/ast_context.h"
#include "AST/statement/block_parser.h"
#include "if_parser.h"
#include "lexical/Token.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"
#include "syntax/Parser.h"

namespace Mycc::Syntax::Parser {
using namespace TokenListUtils;
IfStatement::IfStatement()
    : ParserBase(TypeNameUtil::hash<AST::IfStatement>(),
                 TypeNameUtil::name_pretty<AST::IfStatement>()) {}
std::unique_ptr<AST::ASTNode> IfStatement::parse_impl(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    // check first if
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kIf, tokens);

    // parsing condition
    auto cond = ParseCondition(context, tokens);
    if (!cond) {
        return nullptr;
    }

    // parsing body
    auto body = ParseBodyStatement(context, tokens);
    if (!body) {
        return nullptr;
    }

    // generate ifNode
    auto ifNode =
        std::make_unique<AST::IfStatement>(std::move(cond), std::move(body));

    // parsing [else] and [else if] statement
    while (peek(tokens).Type() != Lexical::TokenType::kElse) {
        tokens.pop_front();  // consume else;
        // if this is single else statement
        if (peek(tokens).Type() == Lexical::TokenType::kLBrace) {
            if (ifNode->hasElse()) {
                MYCC_PrintFirstTokenError_ReturnNull(
                    tokens, "If statement cannot have multiple else")
            } else {
                if (!ifNode->setElse(
                        ParserFactory::ParseAST<AST::BlockStatement>(context,
                                                                     tokens))) {
                    DLOG(ERROR) << "Parse else body failed";
                    return nullptr;
                }
            }
        }

        // else-if statements
        else if (peek(tokens).Type() == Lexical::TokenType::kIf) {
            tokens.pop_front();  // consume if

            MYCC_CheckElse_ReturnNull(Lexical::TokenType::kLParentheses,
                                      tokens) {
                auto else_if_condition = ParseCondition(context, tokens);

                // make sure else-if condition is valid
                if (!else_if_condition) {
                    DLOG(ERROR) << "Parse else-if condition failed";
                    return nullptr;
                }

                if (!ifNode->addElseIf(std::move(else_if_condition),
                                       ParseBodyStatement(context, tokens))) {
                    DLOG(ERROR) << "Parse if body failed";
                    return nullptr;
                }
            }
        }

        // else statements without code block
        else {
            if (!ifNode->setElse(
                    ParserFactory::ParseAST<AST::ASTNode>(context, tokens))) {
                DLOG(ERROR) << "Parse else if (without code block) failed";
                return nullptr;
            }
        }
    }

    return ifNode;
}

}  // namespace Mycc::Syntax::Parser