//
// Created by chen_ on 2022/3/24.
//
//
// Created by chen_ on 2022/3/24.
//
#include <list>

#include "ast/CompilationUnit.h"
#include "ast/Stmt.h"
#include "parser/syntax/parser/syntax_parser.h"
#include "parser/syntax/parser_factory.h"
#include "parser/syntax/utils/common_utils.h"

namespace hzcc::syntax::parser {
ALWAYS_INLINE StatusOr<ast::ExprPtr> ParseExpr(TokenList& tokens,
                                               hzcc::syntax::Ctx& context) {
    if (tokens.peek().Type() != TokenType::kSemiColon) {
        auto tmp_ret = ParserFactory::ParseAST<ast::Expr>(tokens, context);
        if (tmp_ret.ok()) {
            // check ';'  and consume
            HZCC_CheckAndConsume_ReturnErr(TokenType::kSemiColon, tokens);
            return tmp_ret;
        } else {
            return tmp_ret;
        }
    } else {
        // consume ';'
        // check ';'  and consume
        HZCC_CheckAndConsume_ReturnErr(TokenType::kSemiColon, tokens);
        return std::make_unique<ast::EmptyExpr>();
    }
}

StatusOr<ast::StmtPtr> ForStatement::parse_impl(TokenList& tokens,
                                                hzcc::syntax::Ctx& context) {
    EnterLoop();  // enter loop

    // check first token is for
    auto for_loc = tokens.peek().Location();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kFor, tokens);

    // check next token is (
    auto left_paren_loc = tokens.peek().Location();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kLParentheses, tokens);

    // enter new scope
    context.enter_scope();

    // parse initializer

    // parse condition
    std::unique_ptr<ast::Stmt> condition{nullptr};
    if (tokens.peek().Type() != TokenType::kSemiColon) {
        HZCC_CHECK_OR_ASSIGN(condition, ParserFactory::ParseAST<ast::Stmt>(tokens, context));

        // check ';'  and consume
        HZCC_CheckAndConsume_ReturnErr(TokenType::kSemiColon, tokens);
    } else {
        // consume ';'
        tokens.pop();
        condition = std::make_unique<ast::EmptyStmt>();
    }

    // parse increment
    std::unique_ptr<ast::Stmt> increment{nullptr};
    if (tokens.peek().Type() != TokenType::kRParentheses) {
        HZCC_CHECK_OR_ASSIGN(
            increment,  // NOLINT
            ParserFactory::ParseAST<ast::Stmt>(tokens, context));

        // check next token is )
        HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens);
    } else {
        // consume ')'
        tokens.pop();
        increment = std::make_unique<ast::EmptyStmt>();
    }

    // parse body and return
    std::unique_ptr<ast::Stmt> body{nullptr};
    if (tokens.peek().Type() != TokenType::kSemiColon) {
        body = utils::ParseBodyStatement(tokens, context, false);
        if (body == nullptr) return nullptr;
    } else {
        DLOG(WARNING) << "for stmt body is empty";
        body = std::make_unique<ast::EmptyStmt>();
    }

    // leave scope
    context.leave_scope();

    // push a semicolon for easier parsing
    tokens.push(Token(TokenType::kSemiColon, -1, -1));

    ExitLoop();  // exit loop
    return std::make_unique<ast::ForStmt>(
        for_loc, std::move(initializer), std::move(condition),
        std::move(increment), std::move(body));
}

}  // namespace hzcc::syntax::parser
