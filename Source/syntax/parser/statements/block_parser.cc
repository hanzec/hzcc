//
// Created by chen_ on 2022/3/24.
//
#include "AST/statement/block_parser.h"

#include <list>

#include "AST/ast_context.h"
#include "lexical/Token.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"
#include "block_parser.h"
#include "syntax/Parser.h"

namespace Mycc::Syntax::Parser {
BlockStatement::BlockStatement()
    : ParserBase(TypeNameUtil::hash<AST::BlockStatement>(),
                 TypeNameUtil::name_pretty<AST::BlockStatement>()) {}
std::unique_ptr<AST::ASTNode> BlockStatement::parse_impl(          // NOLINT
    AST::ASTContext& context,                      // NOLINT
    std::list<Lexical::Token>& tokens) {  // NOLINT
    // check if the next token is '{'
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLBrace, tokens);

    // create new block node
    auto block_node = std::make_unique<AST::BlockStatement>();

    // parse statements
    while (tokens.front().Type() != Lexical::TokenType::kRBrace) {
        block_node->AddStatement(
            ParserFactory::ParseAST<AST::ASTNode>(context, tokens));
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kSemiColon, tokens);
    }

    // consume '}'
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRBrace, tokens);

    return block_node;
}

}  // namespace Mycc::Syntax::Parser