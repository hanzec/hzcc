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
IfStatement::IfStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::IfStmt>(),
                 TypeNameUtil::name_pretty<ast::IfStmt>()) {}
StatusOr<ast::StmtPtr> IfStatement::parse_impl(SyntaxCtx context,
                                               TokenList& tokens) {
    auto ref = tokens.peek();

    // check first if
    auto if_loc = tokens.peek().loc();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kIf, tokens);

    // parsing condition
    auto cond_token = tokens.peek();
    HZCC_CHECK_OR_ASSIGN(condition,  // NOLINT
                         utils::ParseCondition(context, tokens));

    // if condition could be converted to int
    if (!condition->retType()->IsNumericalType()) {
        return syntax::utils::TokenErr(
            cond_token, "if condition has non-numeric type void");
    }

    // parsing body
    HZCC_CHECK_OR_ASSIGN(body,  // NOLINT
                         utils::ParseBodyStatement(context, false, tokens));

    // generate ifNode
    auto ifNode = std::make_unique<ast::IfStmt>(if_loc, std::move(condition),
                                                std::move(body));

    // parsing [else] and [else if] stmt
    while (tokens.peek().Type() == TokenType::kElse) {
        auto prev_else = tokens.pop();  // consume else;
        // if this is single else stmt
        if (tokens.peek().Type() == TokenType::kLBrace) {
            if (ifNode->HasElse()) {
                return syntax::utils::TokenErr(
                    prev_else, "If stmt cannot have multiple else statements");
            } else {
                // enter new scope
                context->enter_scope();

                // parsing body
                HZCC_CHECK_OR_ASSIGN(
                    else_body,  // NOLINT
                    utils::ParseBodyStatement(context, false, tokens));
                ifNode->set_else(std::move(else_body));

                // leave scope
                context->leave_scope();
            }
        }

        // else-if statements
        else if (tokens.peek().Type() == TokenType::kIf) {
            tokens.pop();  // consume if
            HZCC_CheckAndConsume_ReturnErr(TokenType::kLParentheses, tokens);

            auto else_if_token = tokens.peek();
            HZCC_CheckAndConsume_ReturnErr(TokenType::kLParentheses, tokens);

            HZCC_CHECK_OR_ASSIGN(else_if_condition,
                                 Parser::Parse<ast::Expr>(context, tokens))
            // next token is ')'
            HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens);

            // check if else-if condition is valid
            if (else_if_condition->retType()->is<TypeCategory::kNumerical>()) {
                return syntax::utils::TokenErr(
                    else_if_token, "if condition has non-numeric type void");
            }

            // add else-if stmt
            HZCC_CHECK_OR_ASSIGN(
                else_if_body,  // NOLINT
                utils::ParseBodyStatement(context, false, tokens));
            ifNode->addElseIf(std::move(else_if_condition),
                              std::move(else_if_body));
        }

        // else statements without code block
        else {
            HZCC_CHECK_OR_ASSIGN(
                else_body,  // NOLINT
                utils::ParseBodyStatement(context, false, tokens));
            ifNode->set_else(std::move(else_body));
        }
    }
    return ifNode;
}

}  // namespace hzcc::syntax::parser
