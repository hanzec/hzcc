//
// Created by chen_ on 2022/3/25.
//
#include <list>

#include "ast/CompilationUnit.h"
#include "parser/syntax/parser_factory.h"
#include "parser/syntax/utils/common_utils.h"

namespace hzcc::syntax::parser {
DoStatement::DoStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::DoStmt>(),
                 TypeNameUtil::name_pretty<ast::DoStmt>()) {}

StatusOr<ast::StmtPtr> DoStatement::parse_impl(TokenList& tokens,
                                               hzcc::syntax::Ctx& context) {
    EnterLoop();  // enter loop

    // check if the next token is [do]
    auto do_loc = tokens.peek().Location();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kDo, tokens);

    // parse body stmt
    HZCC_CHECK_OR_ASSIGN(condition,  // NOLINT
                         utils::ParseBodyStatement(tokens, context, false));

    // check if the next token is [while]
    HZCC_CheckAndConsume_ReturnErr(TokenType::kWhile, tokens);

    // parse condition
    HZCC_CHECK_OR_ASSIGN(body,  // NOLINT
                         utils::ParseCondition(tokens, context));

    ExitLoop();  // exit loop
    return std::make_unique<ast::DoStmt>(std::move(condition), std::move(body),
                                         do_loc);
}

}  // namespace hzcc::syntax::parser
