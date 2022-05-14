//
// Created by chen_ on 2022/3/25.
//
//
// Created by chen_ on 2022/3/25.
//

#include "while_parser.h"

#include <list>

#include "AST/statement/compound.h"
#include "AST/statement/do.h"
#include "lexical/Token.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"
#include "utils/type_name_utils.h"

namespace Hzcc::Syntax::Parser {
WhileStatement::WhileStatement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::WhileStatement>(),
                 TypeNameUtil::name_pretty<AST::WhileStatement>()) {}
std::unique_ptr<AST::ASTNode> WhileStatement::parse_impl(
    TokenList& tokens, SyntaxContext& context) {
    EnterLoop();  // enter loop

    // check if the next token is [while]
    auto while_loc = tokens.peek().Location();
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kWhile, tokens);

    // parse condition
    auto condition = ParseCondition(tokens, context);
    if (condition == nullptr) return nullptr;

    // parse body statement
    auto body = ParseBodyStatement(tokens, context, false);
    if (body == nullptr) return nullptr;

    // push a semicolon for easier parsing
    tokens.push(Lexical::Token(Lexical::TokenType::kSemiColon, -1, -1));

    ExitLoop();  // exit loop
    return std::make_unique<AST::WhileStatement>(std::move(condition),
                                                 std::move(body), while_loc);
}

}  // namespace Hzcc::Syntax::Parser
