//
// Created by chen_ on 2022/3/24.
//
//
// Created by chen_ on 2022/3/24.
//
#include "AST/statement/for_parser.h"

#include <list>

#include "AST/ast_context.h"
#include "AST/statement/block_parser.h"
#include "for_parser.h"
#include "lexical/token_type.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"
#include "syntax/Parser.h"

namespace Mycc::Syntax::Parser {
using namespace TokenListUtils;
ForStatement::ForStatement()
    : ParserBase(TypeNameUtil::hash<AST::ForStatement>(),
                 TypeNameUtil::name_pretty<AST::ForStatement>()) {}
std::unique_ptr<AST::ASTNode> ForStatement::parse_impl(  // NOLINT
    AST::ASTContext& context,                            // NOLINT
    std::list<Lexical::Token>& tokens) {                 // NOLINT
    // check first token is for
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kFor, tokens);

    // check next token is (
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses, tokens);

    // parse initializer
    auto initializer = ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
    if (initializer == nullptr) return nullptr;

    // parse condition
    auto condition = ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
    if (condition == nullptr) return nullptr;

    // parse increment
    auto increment = ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
    if (condition == nullptr) return nullptr;

    // check next token is )
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses, tokens);

    // parse body and return
    auto body = ParserFactory::ParseAST<AST::BlockStatement>(context, tokens);
    if (body == nullptr) return nullptr;

    return std::make_unique<AST::ForStatement>(
        std::move(initializer), std::move(condition), std::move(increment),
        std::move(body));
}

}  // namespace Mycc::Syntax::Parser