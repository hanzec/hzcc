//
// Created by chen_ on 2022/3/24.
//

#include <list>

#include "ast/CompilationUnit.h"
#include "parser/syntax/common_utils.h"
#include "parser/syntax/parser/parser_factory.h"
#include "parser/syntax/parser/syntax_parser.h"
namespace hzcc::syntax::parser {
CompoundStatement::CompoundStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::CompoundStmt>(),
                 TypeNameUtil::name_pretty<ast::CompoundStmt>()){};

StatusOr<ast::StmtPtr> CompoundStatement::parse_impl(SyntaxCtx context,
                                                     TokenList& tokens) {
    // check if the next token is '{'
    auto prev_token = tokens.peek();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kLBrace, tokens);

    // create new block node
    auto block_node = std::make_unique<ast::CompoundStmt>(prev_token.loc());

    // parse statements
    while (!tokens.empty() && tokens.peek().Type() != TokenType::kRBrace) {
        HZCC_CHECK_OR_ASSIGN(stmt,  // NOLINT
                             Parser::Parse<ast::Stmt>(context, tokens))
        block_node->AddStatement(std::move(stmt));

        if (!block_node->GetLastStatement()->has_body()) {
            // stmt need end with ';'
            HZCC_CheckAndConsume_ReturnErr(TokenType::kSemiColon, tokens);
        }
    }

    // consume '}'
    if (tokens.empty()) {
        return syntax::utils::TokenErr(prev_token, "Unmatched '{', need '}'");
    } else {
        HZCC_CheckAndConsume_ReturnErr(TokenType::kRBrace, tokens);
    }

    return block_node;
}

}  // namespace hzcc::syntax::parser
