//
// Created by chen_ on 2022/3/25.
//
//
// Created by chen_ on 2022/3/25.
//

#include <list>

#include "AST/CompilationUnit.h"
#include "AST/statement/compound.h"
#include "AST/statement/do.h"
#include "lexical/Token.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "syntax/utils/token_list_utils.h"
#include "utils/message_utils.h"
#include "utils/type_name_utils.h"
#include "while_parser.h"

namespace Hzcc::Syntax::Parser {
WhileStatement::WhileStatement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::WhileStatement>(),
                 TypeNameUtil::name_pretty<AST::WhileStatement>()) {}
std::unique_ptr<AST::ASTNode> WhileStatement::parse_impl(
    AST::CompilationUnit& context, TokenList& tokens) {
    // check if the next token is [while]
    auto while_loc = TokenListUtils::peek(tokens).Location();
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kWhile, tokens);

    // parse condition
    auto condition = ParseCondition(context, tokens);
    if (condition == nullptr) return nullptr;

    // parse body statement
    auto body = ParseBodyStatement(context, tokens);
    if (body == nullptr) return nullptr;

    // push a semicolon for easier parsing
    tokens.push_front(Lexical::Token(Lexical::TokenType::kSemiColon, -1, -1));

    return std::make_unique<AST::WhileStatement>(
        std::move(body), std::move(condition), while_loc);
}

}  // namespace Hzcc::Syntax::Parser
