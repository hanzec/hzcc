//
// Created by chen_ on 2022/3/24.
//
//
// Created by chen_ on 2022/3/24.
//
#include <list>

#include "ast/CompilationUnit.h"
#include "ast/Stmt.h"
#include "parser/syntax/common_utils.h"
#include "parser/syntax/parser/parser_factory.h"
#include "parser/syntax/parser/syntax_parser.h"

namespace hzcc::syntax::parser {
StatusOr<ast::StmtPtr> ForStatement::parse_impl(SyntaxCtx context,
                                                TokenList& tokens) {
    EnterLoop();  // enter loop

    // check first token is for
    auto for_loc = tokens.peek().loc();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kFor, tokens);

    // check next token is (
    auto left_paren_loc = tokens.peek().loc();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kLParentheses, tokens);

    // parse
    auto for_node = std::make_unique<ast::ForStmt>(for_loc);

    // enter new scope
    context->enter_scope();

    // parse initializer
    if (tokens.peek().Type() != TokenType::kSemiColon) {
        HZCC_CHECK_OR_ASSIGN(initializer,  // NOLINT
                             Parser::Parse<ast::Stmt>(context, tokens));
        for_node->set_init(std::move(initializer));

        // check ';'  and consume
        HZCC_CheckAndConsume_ReturnErr(TokenType::kSemiColon, tokens)
    }

    // parse condition
    if (tokens.peek().Type() != TokenType::kSemiColon) {
        HZCC_CHECK_OR_ASSIGN(condition,  // NOLINT
                             Parser::Parse<ast::Stmt>(context, tokens))
        for_node->set_cond(std::move(condition));

        // check ';'  and consume
        HZCC_CheckAndConsume_ReturnErr(TokenType::kSemiColon, tokens);
    }

    // parse increment
    if (tokens.peek().Type() != TokenType::kRParentheses) {
        HZCC_CHECK_OR_ASSIGN(increment,  // NOLINT
                             Parser::Parse<ast::Stmt>(context, tokens));
        for_node->set_incr(std::move(increment));

        // check next token is )
        HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens);
    }

    // parse body and return
    if (tokens.peek().Type() != TokenType::kSemiColon) {
        HZCC_CHECK_OR_ASSIGN(body,  // NOLINT
                             utils::ParseBodyStatement(context, false, tokens))
        for_node->set_body(std::move(body));
    }

    // leave scope
    context->leave_scope();

    // push a semicolon for easier parsing
    tokens.push(TokenType::kSemiColon, -1, -1);

    ExitLoop();  // exit loop
    return for_node;
}

}  // namespace hzcc::syntax::parser
