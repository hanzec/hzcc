//
// Created by chen_ on 2022/3/24.
//
//
// Created by chen_ on 2022/3/24.
//
#include <glog/logging.h>

#include <memory>
#include <utility>

#include "ast/CompilationUnit.h"
#include "ast/Stmt.h"
#include "parser/common/Token.h"
#include "parser/common/token_type.h"
#include "parser/parser.h"
#include "parser/syntax/common_utils.h"
#include "parser/syntax/parser/parser_factory.h"
#include "parser/syntax/parser/syntax_parser.h"
#include "utils/logging.h"
#include "utils/status/statusor.h"

namespace hzcc::syntax::parser {
ForStatement::ForStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::ForStmt>(),
                 TypeNameUtil::name_pretty<ast::ForStmt>()) {}

StatusOr<ast::StmtPtr> ForStatement::parse_impl(SyntaxCtx context,
                                                TokenList& tokens) {
    EnterLoop();  // enter loop

    // check first token is for
    auto for_loc = tokens.peek().loc();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kFor, tokens);

    // check next token is (
    auto left_paren_loc = tokens.peek().loc();
    HZCC_CheckAndConsume_ReturnErr(TokenType::LParentheses, tokens);

    // parse
    auto for_node = std::make_unique<ast::ForStmt>(for_loc);

    // enter new scope
    context->enter_scope();

    // parse initializer
    if (tokens.peek().Type() != TokenType::SemiColon) {
        HZCC_CHECK_OR_ASSIGN(initializer,  // NOLINT
                             Parser::Parse<ast::Stmt>(context, tokens));
        for_node->set_init(std::move(initializer));

        // check ';'  and consume
        HZCC_CheckAndConsume_ReturnErr(TokenType::SemiColon, tokens)
    }

    // parse condition
    if (tokens.peek().Type() != TokenType::SemiColon) {
        HZCC_CHECK_OR_ASSIGN(condition,  // NOLINT
                             Parser::Parse<ast::Stmt>(context, tokens))
        for_node->set_cond(std::move(condition));

        // check ';'  and consume
        HZCC_CheckAndConsume_ReturnErr(TokenType::SemiColon, tokens);
    }

    // parse increment
    if (tokens.peek().Type() != TokenType::RParentheses) {
        HZCC_CHECK_OR_ASSIGN(increment,  // NOLINT
                             Parser::Parse<ast::Stmt>(context, tokens));
        for_node->set_incr(std::move(increment));

        // check next token is )
        HZCC_CheckAndConsume_ReturnErr(TokenType::RParentheses, tokens);
    }

    // parse body and return
    if (tokens.peek().Type() != TokenType::SemiColon) {
        HZCC_CHECK_OR_ASSIGN(body,  // NOLINT
                             utils::ParseBodyStatement(context, false, tokens))
        for_node->set_body(std::move(body));
    }

    // leave scope
    context->leave_scope();

    // push a semicolon for easier parsing
    tokens.push(TokenType::SemiColon, -1, -1);

    ExitLoop();  // exit loop
    return for_node;
}

}  // namespace hzcc::syntax::parser
