//
// Created by chen_ on 2022/3/25.
//
#include <list>

#include "ast/CompilationUnit.h"
#include "parser/syntax/common_utils.h"
#include "parser/syntax/parser/parser_factory.h"

namespace hzcc::syntax::parser {
DoStatement::DoStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::DoStmt>(),
                 TypeNameUtil::name_pretty<ast::DoStmt>()) {}

StatusOr<ast::StmtPtr> DoStatement::parse_impl(SyntaxCtx context,
                                               TokenList& tokens) {
    EnterLoop();  // enter loop

    // check if the next token is [do]
    auto do_loc = tokens.peek().loc();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kDo, tokens);

    // parse body stmt
    HZCC_CHECK_OR_ASSIGN(condition,  // NOLINT
                         utils::ParseBodyStatement(context, false, tokens));

    // check if the next token is [while]
    HZCC_CheckAndConsume_ReturnErr(TokenType::kWhile, tokens);

    // parse condition
    HZCC_CHECK_OR_ASSIGN(body,  // NOLINT
                         utils::ParseCondition(context, tokens));

    ExitLoop();  // exit loop
    return std::make_unique<ast::DoStmt>(do_loc, std::move(condition),
                                         std::move(body));
}

}  // namespace hzcc::syntax::parser
