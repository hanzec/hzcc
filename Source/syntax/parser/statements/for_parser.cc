//
// Created by chen_ on 2022/3/24.
//
//
// Created by chen_ on 2022/3/24.
//
#include "for_parser.h"

#include <list>

#include "AST/CompilationUnit.h"
#include "AST/stmt/CompoundStmt.h"
#include "AST/stmt/EmptyStmt.h"
#include "AST/stmt/ForStmt.h"
#include "lexical/token_type.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"

namespace Hzcc::Syntax::Parser {
ForStatement::ForStatement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::ForStmt>(),
                 TypeNameUtil::name_pretty<AST::ForStmt>()) {}
std::unique_ptr<AST::ASTNode> ForStatement::parse_impl(TokenList& tokens,
                                                       SyntaxContext& context) {
    EnterLoop();  // enter loop

    // check first token is for
    auto for_loc = tokens.peek().Location();
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kFor, tokens);

    // check next token is (
    auto left_paren_loc = tokens.peek().Location();
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses, tokens);

    // enter new scope
    context.enterScope();

    // parse initializer
    std::unique_ptr<AST::ASTNode> initializer{nullptr};
    if (tokens.peek().Type() != Lexical::TokenType::kSemiColon) {
        initializer = ParserFactory::ParseAST<AST::ASTNode>(tokens, context);
        if (initializer == nullptr) {
            DLOG(ERROR) << "initializer is not nullptr";
            return nullptr;
        }
        // check ';'  and consume
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kSemiColon, tokens);
    } else {
        // consume ';'
        initializer =
            std::make_unique<AST::EmptyStatement>(tokens.pop().Location());
    }

    // parse condition
    std::unique_ptr<AST::ASTNode> condition{nullptr};
    if (tokens.peek().Type() != Lexical::TokenType::kSemiColon) {
        condition = ParserFactory::ParseAST<AST::ASTNode>(tokens, context);
        if (condition == nullptr) {
            DLOG(ERROR) << "condition is not nullptr";
            return nullptr;
        }

        // check ';'  and consume
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kSemiColon, tokens);
    } else {
        // consume ';'
        condition =
            std::make_unique<AST::EmptyStatement>(tokens.pop().Location());
    }

    // parse increment
    std::unique_ptr<AST::ASTNode> increment{nullptr};
    if (tokens.peek().Type() != Lexical::TokenType::kRParentheses) {
        increment = ParserFactory::ParseAST<AST::ASTNode>(tokens, context);
        if (increment == nullptr) {
            DLOG(ERROR) << "increment parse failed is not nullptr";
            return nullptr;
        }
        // check next token is )
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses,
                                        tokens);
    } else {
        // consume ')'
        increment =
            std::make_unique<AST::EmptyStatement>(tokens.pop().Location());
    }

    // parse body and return
    std::unique_ptr<AST::ASTNode> body{nullptr};
    if (tokens.peek().Type() != Lexical::TokenType::kSemiColon) {
        body = ParseBodyStatement(tokens, context, false);
        if (body == nullptr) return nullptr;
    } else {
        DLOG(WARNING) << "for stmt body is empty";
    }

    // leave scope
    context.leaveScope();

    // push a semicolon for easier parsing
    tokens.push(Lexical::Token(Lexical::TokenType::kSemiColon, -1, -1));

    ExitLoop();  // exit loop
    return std::make_unique<AST::ForStmt>(
        for_loc, std::move(initializer), std::move(condition),
        std::move(increment), std::move(body));
}

}  // namespace Hzcc::Syntax::Parser
