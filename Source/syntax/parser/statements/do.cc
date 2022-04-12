//
// Created by chen_ on 2022/3/25.
//

#include "AST/statement/do.h"

#include <list>

#include "AST/ASTContext.h"
#include "do_parser.h"
#include "lexical/Token.h"
#include "syntax/Parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"

namespace Mycc::Syntax::Parser {
DoStatement::DoStatement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::DoStatement>(),
                 TypeNameUtil::name_pretty<AST::DoStatement>()) {}

std::unique_ptr<AST::ASTNode> DoStatement::parse_impl(AST::ASTContext& context,
                                                      TokenList& tokens) {
    // check if the next token is [do]
    auto do_loc = tokens.front().Location();
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kDo, tokens);

    // parse body statement
    auto body = ParseBodyStatement(context, tokens, false);
    if (body == nullptr) return nullptr;

    // check if the next token is [while]
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kWhile, tokens);

    // parse condition
    auto condition = ParseCondition(context, tokens);
    if (condition == nullptr) return nullptr;

    return std::make_unique<AST::DoStatement>(std::move(body),
                                              std::move(condition), do_loc);
}

}  // namespace Mycc::Syntax::Parser
