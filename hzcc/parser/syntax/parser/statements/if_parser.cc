//
// Created by chen_ on 2022/3/24.
//

#include <glog/logging.h>
#include <memory>
#include <utility>

#include "ast/CompilationUnit.h"
#include "ast/type/Type.h"
#include "parser/syntax/common_utils.h"
#include "parser/syntax/parser/parser_factory.h"
#include "parser/syntax/parser/syntax_parser.h"
#include "ast/Stmt.h"
#include "enums.h"
#include "parser/common/Token.h"
#include "parser/common/token_type.h"
#include "parser/parser.h"
#include "utils/logging.h"
#include "utils/status/status.h"
#include "utils/status/statusor.h"
#include "utils/type_name_utils.h"

namespace hzcc::syntax::parser {
namespace {
/** @brief Parse an if or else-if condition and diagnose a known non-scalar type. */
StatusOr<ast::ExprPtr> ParseIfCondition(ParserBase::SyntaxCtx& context, TokenList& tokens,
                                        const Token& keyword) {
    if (tokens.empty()) return utils::TokenErr(keyword, "Expected '(' after 'if'");
    const auto opening = tokens.peek();
    HZCC_CHECK_OR_ASSIGN(condition, utils::ParseCondition(context, tokens));
    const auto type = condition->type();
    if (type && (ast::IsTypeOf<PrimitiveType::kVoid>(type) ||
                 type->is<TypeCategory::Struct>() || type->is<TypeCategory::Union>())) {
        return utils::TokenErr(opening,
                               "if condition has non-numeric type " + type->to_str());
    }
    return condition;
}
}  // namespace

IfStatement::IfStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::IfStmt>(),
                 TypeNameUtil::name_pretty<ast::IfStmt>()) {}
StatusOr<ast::StmtPtr> IfStatement::parse_impl(SyntaxCtx context,
                                               TokenList& tokens) {

    // check first if
    auto keyword = tokens.peek();
    HZCC_CheckAndConsume_ReturnErr(TokenType::If, tokens);

    // parsing condition
    HZCC_CHECK_OR_ASSIGN(cond,  // NOLINT
                         ParseIfCondition(context, tokens, keyword));

    // parsing body
    HZCC_CHECK_OR_ASSIGN(body,  // NOLINT
                         utils::ParseBodyStatement(context, tokens));

    // generate ifNode
    auto node = std::make_unique<ast::IfStmt>(keyword.loc(), std::move(cond),
                                              std::move(body));

    // parsing [else] and [else if] stmt
    while (!tokens.empty() && tokens.peek().type() == TokenType::Else) {
        auto alternative_keyword = tokens.pop();  // consume else;
        if (tokens.empty()) {
            return utils::TokenErr(alternative_keyword,
                                   "Expected statement after 'else'");
        }

        // if this is single else stmt
        if (tokens.peek().type() != TokenType::If) {
            HZCC_CHECK_OR_ASSIGN(alternative, utils::ParseBodyStatement(context, tokens));
            node->set_else(std::move(alternative));
            break;
        }

        // else-if statements
        keyword = tokens.pop();  // consume if

        // check if else-if condition is valid
        HZCC_CHECK_OR_ASSIGN(else_if_cond, ParseIfCondition(context, tokens, keyword));
        HZCC_CHECK_OR_ASSIGN(else_if_body, utils::ParseBodyStatement(context, tokens));

        // add else-if stmt
        node->add_else_if(std::move(else_if_cond), std::move(else_if_body));
    }
    return node;
}

}  // namespace hzcc::syntax::parser
