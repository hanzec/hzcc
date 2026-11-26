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
    context->enter_loop();  // enter loop
    absl::Cleanup leave_loop = [&] { context->leave_loop(); };  // exit loop

    // check first token is for
    auto keyword = tokens.peek();
    HZCC_CheckAndConsume_ReturnErr(TokenType::For, tokens);

    // check next token is (
    const auto opening = tokens.empty() ? keyword : tokens.peek();
    HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::LParentheses, tokens, keyword));

    // enter new scope
    context->enter_scope();
    // leave scope
    absl::Cleanup leave_scope = [&] { context->leave_scope(); };

    // parse
    auto node = std::make_unique<ast::ForStmt>(keyword.loc());

    // parse initializer
    if (tokens.empty()) return utils::Consume(TokenType::SemiColon, tokens, opening);
    auto first = tokens.peek();
    if (is_type_specifier(first.type()) || first.type() == TokenType::Struct ||
        first.type() == TokenType::Enum || first.type() == TokenType::Union ||
        (first.type() == TokenType::kIdentity && context->has_type(first.to_str()))) {
        HZCC_CHECK_OR_ASSIGN(init, Parser::Parse<ast::IDeclStmt>(context, tokens));
        node->set_init(std::move(init));
    } else {
        if (first.type() != TokenType::SemiColon) {
            HZCC_CHECK_OR_ASSIGN(init, Parser::Parse<ast::Expr>(context, tokens));
            node->set_init(std::move(init));
        }

        // check ';'  and consume
        HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::SemiColon, tokens, opening));
    }

    // parse condition
    if (!tokens.empty() && tokens.peek().type() != TokenType::SemiColon) {
        HZCC_CHECK_OR_ASSIGN(cond, Parser::Parse<ast::Expr>(context, tokens));
        node->set_cond(std::move(cond));
    }

    // check ';'  and consume
    HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::SemiColon, tokens, opening));

    // parse increment
    if (!tokens.empty() && tokens.peek().type() != TokenType::RParentheses) {
        HZCC_CHECK_OR_ASSIGN(incr, Parser::Parse<ast::Expr>(context, tokens));
        node->set_incr(std::move(incr));
    }

    // check next token is )
    HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::RParentheses, tokens, opening));

    // parse body and return
    HZCC_CHECK_OR_ASSIGN(body,  // NOLINT
                         utils::ParseBodyStatement(context, tokens));
    node->set_body(std::move(body));
    return node;
}

}  // namespace hzcc::syntax::parser
