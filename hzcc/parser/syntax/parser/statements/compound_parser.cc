//
// Created by chen_ on 2022/3/24.
//

#include <glog/logging.h>
#include <memory>
#include <utility>

#include "parser/syntax/common_utils.h"
#include "parser/syntax/parser/parser_factory.h"
#include "parser/syntax/parser/syntax_parser.h"
#include "ast/Stmt.h"
#include "parser/common/Token.h"
#include "parser/common/token_type.h"
#include "parser/parser.h"
#include "utils/logging.h"
#include "utils/status/status.h"
#include "utils/status/statusor.h"
#include "utils/type_name_utils.h"

namespace hzcc::syntax::parser {
CompoundStatement::CompoundStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::CompoundStmt>(),
                 TypeNameUtil::name_pretty<ast::CompoundStmt>()){};

StatusOr<ast::StmtPtr> CompoundStatement::parse_impl(SyntaxCtx context,
                                                     TokenList& tokens) {
    // check if the next token is '{'
    auto prev_token = tokens.peek();
    HZCC_CheckAndConsume_ReturnErr(TokenType::LBrace, tokens);

    // create new block node
    auto block_node = std::make_unique<ast::CompoundStmt>(prev_token.loc());

    // parse statements
    while (!tokens.empty() && tokens.peek().Type() != TokenType::RBrace) {
        HZCC_CHECK_OR_ASSIGN(stmt,  // NOLINT
                             Parser::Parse<ast::Stmt>(context, tokens))
        block_node->add_stmt(std::move(stmt));

        if (!block_node->last_stmt()->has_body()) {
            // stmt need end with ';'
            HZCC_CheckAndConsume_ReturnErr(TokenType::SemiColon, tokens);
        }
    }

    // consume '}'
    if (tokens.empty()) {
        return syntax::utils::TokenErr(prev_token, "Unmatched '{', need '}'");
    } else {
        HZCC_CheckAndConsume_ReturnErr(TokenType::RBrace, tokens);
    }

    return block_node;
}

}  // namespace hzcc::syntax::parser
