//
// Created by chen_ on 2022/3/25.
//
//
// Created by chen_ on 2022/3/25.
//

#include "while_parser.h"

#include <list>

#include "AST/ast_context.h"
#include "AST/statement/block_parser.h"
#include "AST/statement/do_parser.h"
#include "lexical/Token.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"
#include "utils/message_utils.h"
#include "utils/type_name_utils.h"

namespace Mycc::Syntax::Parser {
WhileStatement::WhileStatement()
    : ParserBase(TypeNameUtil::hash<AST::WhileStatement>(),
                 TypeNameUtil::name_pretty<AST::WhileStatement>()) {}
std::unique_ptr<AST::ASTNode> WhileStatement::parse_impl(  // NOLINT
    AST::ASTContext& context,                              // NOLINT
    std::list<Lexical::Token>& tokens) {                   // NOLINT

    // check if the next token is [while]
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kWhile, tokens);

    // parse condition
    auto condition = ParseCondition(context, tokens);
    if (condition == nullptr) return nullptr;

    // parse body statement
    auto body = ParseBodyStatement(context, tokens);
    if (body == nullptr) return nullptr;

    return std::make_unique<AST::WhileStatement>(std::move(body),
                                                 std::move(condition));
}

}  // namespace Mycc::Syntax::Parser