//
// Created by chen_ on 2022/3/24.
//
#include "AST/statement/compound.h"

#include <list>

#include "AST/ASTContext.h"
#include "block_parser.h"
#include "lexical/Token.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"

namespace Mycc::Syntax::Parser {
BlockStatement::BlockStatement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::CompoundStmt>(),
                 TypeNameUtil::name_pretty<AST::CompoundStmt>()){};

std::unique_ptr<AST::ASTNode> BlockStatement::parse_impl(
    AST::ASTContext& context, TokenList& tokens) {
    // check if the next token is '{'
    auto prev_token = tokens.front();
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLBrace, tokens);

    // create new block node
    auto block_node =
        std::make_unique<AST::CompoundStmt>(prev_token.Location());

    // parse statements
    while (!tokens.empty() &&
           tokens.front().Type() != Lexical::TokenType::kRBrace) {
        if (!block_node->AddStatement(
                ParserFactory::ParseAST<AST::ASTNode>(context, tokens))) {
            DVLOG(SYNTAX_LOG_LEVEL) << "Parse block statement error";
            return nullptr;
        }

        if (!block_node->GetLastStatement()->HasBody()) {
            // statement need end with ';'
            MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kSemiColon,
                                            tokens);
        }
    }

    // consume '}'
    if (tokens.empty()) {
        MYCC_PrintTokenError_ReturnNull(prev_token, "Unmatched '{', need '}'");
    } else {
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRBrace, tokens);
    }

    return block_node;
}

}  // namespace Mycc::Syntax::Parser