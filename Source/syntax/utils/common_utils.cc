//
// Created by chen_ on 2022/3/24.
//

#include "common_utils.h"

#include "lexical/token_type.h"
#include "token_list_utils.h"
#include "utils/message_utils.h"
#include "AST/statement/block_parser.h"
#include "syntax/Parser.h"

namespace Mycc::Syntax::Parser {
std::unique_ptr<AST::ASTNode> ParseCondition(AST::ASTContext& context,
                                             std::list<Lexical::Token> tokens) {
    // next token is (
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses, tokens);

    // parse condition
    auto condition = ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
    if (!condition) {
        DLOG(ERROR) << "Parse condition failed";
        return nullptr;
    }

    // next token is )
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses, tokens);
}

std::unique_ptr<AST::ASTNode> ParseBodyStatement(
    AST::ASTContext& context, std::list<Lexical::Token> tokens) {
    if (TokenListUtils::peek(tokens).Type() == Lexical::TokenType::kLBrace) {
        return ParserFactory::ParseAST<AST::BlockStatement>(context, tokens);
    } else {
        return ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
    }
}
}  // namespace Mycc::Syntax::Parser
