//
// Created by chen_ on 2022/3/24.
//
//
// Created by chen_ on 2022/3/24.
//
#include "AST/statement/for.h"

#include <list>

#include "AST/ASTContext.h"
#include "AST/statement/compound.h"
#include "for_parser.h"
#include "lexical/token_type.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"

namespace Mycc::Syntax::Parser {
using namespace TokenListUtils;
ForStatement::ForStatement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::ForStatement>(),
                 TypeNameUtil::name_pretty<AST::ForStatement>()) {}
std::unique_ptr<AST::ASTNode> ForStatement::parse_impl(AST::ASTContext& context,
                                                       TokenList& tokens) {
    // check first token is for
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kFor, tokens);

    // check next token is (
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses, tokens);

    // enter new scope
    context.enterScope();

    // parse initializer
    std::unique_ptr<AST::ASTNode> initializer{nullptr};
    if (peek(tokens).Type() != Lexical::TokenType::kSemiColon) {
        initializer = ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
        if (initializer == nullptr) {
            DLOG(ERROR) << "initializer is not nullptr";
            return nullptr;
        }
        // check ';'  and consume
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kSemiColon,
                                        tokens);
    } else {
        // consume ';'
        pop_list(tokens);
    }

    // parse condition
    std::unique_ptr<AST::ASTNode> condition{nullptr};
    if (peek(tokens).Type() != Lexical::TokenType::kSemiColon) {
        condition = ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
        if (condition == nullptr) {
            DLOG(ERROR) << "condition is not nullptr";
            return nullptr;
        }

        // check ';'  and consume
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kSemiColon,
                                        tokens);
    } else {
        // consume ';'
        pop_list(tokens);
    }

    // parse increment
    std::unique_ptr<AST::ASTNode> increment{nullptr};
    if (peek(tokens).Type() != Lexical::TokenType::kRParentheses) {
        increment = ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
        if (increment == nullptr) {
            DLOG(ERROR) << "increment parse failed is not nullptr";
            return nullptr;
        }
        // check next token is )
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses,
                                        tokens);
    } else {
        pop_list(tokens);
    }

    // parse body and return
    std::unique_ptr<AST::ASTNode> body{nullptr};
    if (peek(tokens).Type() != Lexical::TokenType::kSemiColon) {
        body = ParseBodyStatement(context, tokens);
        if (body == nullptr) return nullptr;
    } else {
        DLOG(WARNING) << "for statement body is empty";
    }

    // leave scope
    context.leaveScope();

    // push a semicolon for easier parsing
    tokens.push_front(Lexical::Token(Lexical::TokenType::kSemiColon, -1, -1));

    return std::make_unique<AST::ForStatement>(
        std::move(initializer), std::move(condition), std::move(increment),
        std::move(body));
}

}  // namespace Mycc::Syntax::Parser