//
// Created by chen_ on 2022/3/25.
//

#include "do_parser.h"

#include <list>

#include "AST/CompilationUnit.h"
#include "AST/stmt/DoStmt.h"
#include "lexical/Token.h"
#include "syntax/Parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"

namespace Hzcc::Syntax::Parser {
DoStatement::DoStatement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::DoStatement>(),
                 TypeNameUtil::name_pretty<AST::DoStatement>()) {}

std::unique_ptr<AST::ASTNode> DoStatement::parse_impl(TokenList& tokens,
                                                      SyntaxContext& context) {
    EnterLoop();  // enter loop

    // check if the next token is [do]
    auto do_loc = tokens.peek().Location();
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kDo, tokens);

    // parse body stmt
    auto body = ParseBodyStatement(tokens, context, false);
    if (body == nullptr) return nullptr;

    // check if the next token is [while]
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kWhile, tokens);

    // parse condition
    auto condition = ParseCondition(tokens, context);
    if (condition == nullptr) return nullptr;

    ExitLoop();  // exit loop
    return std::make_unique<AST::DoStatement>(std::move(condition),
                                              std::move(body), do_loc);
}

}  // namespace Hzcc::Syntax::Parser
