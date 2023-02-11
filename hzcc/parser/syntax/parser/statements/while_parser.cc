//
// Created by chen_ on 2022/3/25.
//
//
// Created by chen_ on 2022/3/25.
//
#include <glog/logging.h>
#include <memory>
#include <utility>

#include "parser/syntax/common_utils.h"
#include "parser/syntax/parser/syntax_parser.h"
#include "utils/type_name_utils.h"
#include "ast/Stmt.h"
#include "parser/common/Token.h"
#include "parser/common/token_type.h"
#include "parser/parser.h"
#include "utils/logging.h"
#include "utils/status/statusor.h"

namespace hzcc::syntax::parser {
WhileStatement::WhileStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::WhileStmt>(),
                 TypeNameUtil::name_pretty<ast::WhileStmt>()) {}
StatusOr<ast::StmtPtr> WhileStatement::parse_impl(SyntaxCtx context,
                                                  TokenList& tokens) {
    EnterLoop();  // enter loop

    // check if the next token is [while]
    auto while_loc = tokens.peek().loc();
    HZCC_CheckAndConsume_ReturnErr(TokenType::While, tokens);

    // parse condition
    HZCC_CHECK_OR_ASSIGN(condition,  // NOLINT
                         utils::ParseCondition(context, tokens));

    // parse body stmt
    HZCC_CHECK_OR_ASSIGN(body,  // NOLINT
                         utils::ParseBodyStatement(context, false, tokens));

    // push a semicolon for easier parsing
    tokens.push(TokenType::SemiColon, -1, -1);

    ExitLoop();  // exit loop
    return std::make_unique<ast::WhileStmt>(while_loc, std::move(condition),
                                            std::move(body));
}

}  // namespace hzcc::syntax::parser
