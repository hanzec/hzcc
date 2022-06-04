//
// Created by chen_ on 2022/3/24.
//
#include "if_parser.h"

#include <list>

#include "AST/CompilationUnit.h"
#include "AST/stmt/CompoundStmt.h"
#include "AST/stmt/IfStmt.h"
#include "AST/type/Type.h"
#include "lexical/Token.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"

namespace Hzcc::Syntax::Parser {
IfStatement::IfStatement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::IfStmt>(),
                 TypeNameUtil::name_pretty<AST::IfStmt>()) {}
std::unique_ptr<AST::ASTNode> IfStatement::parse_impl(TokenList& tokens,
                                                      SyntaxContext& context) {
    auto ref = tokens.peek();

    // check first if
    auto if_loc = tokens.peek().Location();
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kIf, tokens);

    // parsing condition
    auto cond_token = tokens.peek();
    auto cond = ParseCondition(tokens, context);
    if (!cond) {
        return nullptr;
    }

    // if condition could be converted to int
    if (cond->RetType()->IsVoid()) {
        MYCC_PrintTokenError_ReturnNull(
            cond_token, "if condition has non-numeric type void");
    }

    // parsing body
    auto body = ParseBodyStatement(tokens, context, false);
    if (!body) {
        return nullptr;
    }

    // generate ifNode
    auto ifNode =
        std::make_unique<AST::IfStmt>(if_loc, std::move(cond), std::move(body));

    // parsing [else] and [else if] stmt
    while (tokens.peek().Type() == Lexical::TokenType::kElse) {
        auto prev_else = tokens.pop();  // consume else;
        // if this is single else stmt
        if (tokens.peek().Type() == Lexical::TokenType::kLBrace) {
            if (ifNode->HasElse()) {
                MYCC_PrintTokenError_ReturnNull(
                    prev_else, "If stmt cannot have multiple else statements")
            } else {
                // enter new scope
                context.enterScope();

                if (!ifNode->setElse(ParserFactory::ParseAST<AST::CompoundStmt>(
                        tokens, context))) {
                    DLOG(ERROR) << "Parse else body failed";
                    return nullptr;
                }

                // leave scope
                context.leaveScope();
            }
        }

        // else-if statements
        else if (tokens.peek().Type() == Lexical::TokenType::kIf) {
            tokens.pop();  // consume if
            MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses,
                                            tokens);

            auto else_if_token = tokens.peek();
            auto else_if_condition = ParseCondition(tokens, context);

            // make sure else-if condition is valid
            if (!else_if_condition) {
                DLOG(ERROR) << "Parse else-if condition failed";
                return nullptr;
            }

            // check if else-if condition is valid
            if (else_if_condition->RetType()->IsVoid()) {
                MYCC_PrintTokenError_ReturnNull(
                    else_if_token, "if condition has non-numeric type void");
            }

            // add else-if stmt
            if (!ifNode->addElseIf(
                    std::move(else_if_condition),
                    ParseBodyStatement(tokens, context, false))) {
                DLOG(ERROR) << "Parse if body failed";
                return nullptr;
            }

        }

        // else statements without code block
        else {
            if (!ifNode->setElse(ParseBodyStatement(tokens, context, false))) {
                DLOG(ERROR) << "Parse else if (without code block) failed";
                return nullptr;
            }
        }
    }

    return ifNode;
}

}  // namespace Hzcc::Syntax::Parser
