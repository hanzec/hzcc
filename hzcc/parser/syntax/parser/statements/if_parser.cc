//
// Created by chen_ on 2022/3/24.
//

#include <list>

#include "ast/CompilationUnit.h"
#include "ast/type/Type.h"
#include "parser/syntax/parser/syntax_parser.h"
#include "parser/syntax/parser_factory.h"
#include "parser/syntax/utils/common_utils.h"

namespace hzcc::syntax::parser {
IfStatement::IfStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::IfStmt>(),
                 TypeNameUtil::name_pretty<ast::IfStmt>()) {}
StatusOr<ast::StmtPtr> IfStatement::parse_impl(TokenList& tokens,
                                               hzcc::syntax::Ctx& context) {
    auto ref = tokens.peek();

    // check first if
    auto if_loc = tokens.peek().Location();
    HZCC_CheckAndConsume_ReturnErr(TokenType::kIf, tokens);

    // parsing condition
    auto cond_token = tokens.peek();
    HZCC_CHECK_OR_ASSIGN(condition,  // NOLINT
                         utils::ParseCondition(tokens, context));

    // if condition could be converted to int
    if (!condition->retType()->IsNumericalType()) {
        return syntax::utils::TokenErr(
            cond_token, "if condition has non-numeric type void");
    }

    // parsing body
    HZCC_CHECK_OR_ASSIGN(body,  // NOLINT
                         utils::ParseBodyStatement(tokens, context, false));

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
                context.enter_scope();

                // parsing body
                HZCC_CHECK_OR_ASSIGN(
                    else_body,  // NOLINT
                    utils::ParseBodyStatement(tokens, context, false));
                ifNode->set_else(std::move(else_body));

                // leave scope
                context.leave_scope();
            }
        }

        // else-if statements
        else if (tokens.peek().Type() == TokenType::kIf) {
            tokens.pop();  // consume if
            HZCC_CheckAndConsume_ReturnErr(TokenType::kLParentheses, tokens);

            auto else_if_token = tokens.peek();
            HZCC_CHECK_OR_ASSIGN(else_if_condition,  // NOLINT
                                 utils::ParseCondition(tokens, context));

            // make sure else-if condition is valid
            if (!else_if_condition) {
                DLOG(ERROR) << "Parse else-if condition failed";
                return nullptr;
            }

            // check if else-if condition is valid
            if (else_if_condition->retType()
                    ->is<ast::TypeCategory::kNumerical>()) {
                return syntax::utils::TokenErr(
                    else_if_token, "if condition has non-numeric type void");
            }

            // add else-if stmt
            HZCC_CHECK_OR_ASSIGN(
                else_if_body,  // NOLINT
                utils::ParseBodyStatement(tokens, context, false));
            ifNode->addElseIf(std::move(else_if_condition),
                              std::move(else_if_body));
        }

        // else statements without code block
        else {
            if (!ifNode->set_else(
                    utils::ParseBodyStatement(tokens, context, false))) {
                DLOG(ERROR) << "Parse else if (without code block) failed";
                return nullptr;
            }
        }
    }

    return ifNode;
}

}  // namespace hzcc::syntax::parser
