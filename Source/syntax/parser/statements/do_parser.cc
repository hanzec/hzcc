//
// Created by chen_ on 2022/3/25.
//


#include <list>

#include "AST/ast_context.h"
#include "lexical/Token.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"
#include "do_parser.h"
#include "AST/statement/do_parser.h"
#include "syntax/Parser.h"

namespace Mycc::Syntax::Parser {
DoStatement::DoStatement()
    : ParserBase(TypeNameUtil::hash<AST::DoStatement>(),
                 TypeNameUtil::name_pretty<AST::DoStatement>()) {}

std::unique_ptr<AST::ASTNode> DoStatement::parse_impl(  // NOLINT
    AST::ASTContext& context,                           // NOLINT
    std::list<Lexical::Token>& tokens) {                // NOLINT

    // check if the next token is [do]
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kDo, tokens);

    // parse body statement
    auto body = ParseBodyStatement(context, tokens);
    if (body == nullptr) return nullptr;

    // check if the next token is [while]
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kWhile, tokens);

    // parse condition
    auto condition = ParseCondition(context, tokens);
    if (condition == nullptr) return nullptr;

    return std::make_unique<AST::DoStatement>(std::move(body),
                                              std::move(condition));
}

}  // namespace Mycc::Syntax::Parser