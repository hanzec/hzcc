//
// Created by chen_ on 2022/3/24.
//

#include <list>

#include "ast/CompilationUnit.h"
#include "parser/syntax/parser/syntax_parser.h"
#include "parser/syntax/parser_factory.h"
#include "parser/syntax/utils/common_utils.h"
namespace hzcc::syntax::parser {
CompoundStatement::CompoundStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::CompoundStmt>(),
                 TypeNameUtil::name_pretty<ast::CompoundStmt>()){};

StatusOr<ast::StmtPtr> CompoundStatement::parse_impl(TokenList& tokens,
                                                     Ctx& context) {
    // check if the next token is '{'
    auto prev_token = tokens.peek();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kLBrace, tokens);

    // create new block node
    auto block_node =
        std::make_unique<ast::CompoundStmt>(prev_token.Location());

    // parse statements
    while (!tokens.empty() && tokens.peek().Type() != TokenType::kRBrace) {
        auto stmt = ParserFactory::ParseAST<ast::Stmt>(tokens, context);
        if (!block_node->AddStatement()) {
            DVLOG(SYNTAX_LOG_LEVEL) << "Parse block stmt error";
            return nullptr;
        }

        if (!block_node->GetLastStatement()->HasBody()) {
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
