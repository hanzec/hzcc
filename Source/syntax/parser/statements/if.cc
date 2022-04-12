//
// Created by chen_ on 2022/3/24.
//
#include "AST/statement/if.h"

#include <list>

#include "AST/ASTContext.h"
#include "AST/statement/compound.h"
#include "if_parser.h"
#include "lexical/Token.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"

namespace Mycc::Syntax::Parser {
using namespace TokenListUtils;
IfStatement::IfStatement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::IfStatement>(),
                 TypeNameUtil::name_pretty<AST::IfStatement>()) {}
std::unique_ptr<AST::ASTNode> IfStatement::parse_impl(AST::ASTContext& context,
                                                      TokenList& tokens) {
    auto ref = tokens.front();

    // check first if
    auto if_loc = tokens.front().Location();
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kIf, tokens);

    // parsing condition
    auto cond = ParseCondition(context, tokens);
    if (!cond) {
        return nullptr;
    }

    // parsing body
    auto body = ParseBodyStatement(context, tokens, false);
    if (!body) {
        return nullptr;
    }

    // generate ifNode
    auto ifNode = std::make_unique<AST::IfStatement>(std::move(cond),
                                                     std::move(body), if_loc);

    // parsing [else] and [else if] statement
    while (peek(tokens).Type() == Lexical::TokenType::kElse) {
        auto prev_else = pop_list(tokens);  // consume else;
        // if this is single else statement
        if (peek(tokens).Type() == Lexical::TokenType::kLBrace) {
            if (ifNode->hasElse()) {
                MYCC_PrintTokenError_ReturnNull(
                    prev_else,
                    "If statement cannot have multiple else statements")
            } else {
                // enter new scope
                context.enterScope();

                if (!ifNode->setElse(ParserFactory::ParseAST<AST::CompoundStmt>(
                        context, tokens))) {
                    DLOG(ERROR) << "Parse else body failed";
                    return nullptr;
                }

                // leave scope
                context.leaveScope();
            }
        }

        // else-if statements
        else if (peek(tokens).Type() == Lexical::TokenType::kIf) {
            pop_list(tokens);  // consume if

            MYCC_CheckElse_ReturnNull(Lexical::TokenType::kLParentheses,
                                      tokens) {
                auto else_if_condition = ParseCondition(context, tokens);

                // make sure else-if condition is valid
                if (!else_if_condition) {
                    DLOG(ERROR) << "Parse else-if condition failed";
                    return nullptr;
                }

                if (!ifNode->addElseIf(
                        std::move(else_if_condition),
                        ParseBodyStatement(context, tokens, false))) {
                    DLOG(ERROR) << "Parse if body failed";
                    return nullptr;
                }
            }
        }

        // else statements without code block
        else {
            if (!ifNode->setElse(ParseBodyStatement(context, tokens, false))) {
                DLOG(ERROR) << "Parse else if (without code block) failed";
                return nullptr;
            }
        }
    }

    return ifNode;
}

}  // namespace Mycc::Syntax::Parser
