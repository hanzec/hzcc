//
// Created by chen_ on 2022/3/24.
//
#include "function_parser.h"

#include <list>

#include "AST/ast_context.h"
#include "AST/function_node.h"
#include "AST/statement/block_parser.h"
#include "lexical/Token.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"

namespace Mycc::Syntax::Parser {

using namespace TokenListUtils;
Function::Function()
    : ParserBase(TypeNameUtil::hash<AST::FunctionNode>(),
                 TypeNameUtil::name_pretty<AST::FunctionNode>()) {}

std::unique_ptr<AST::ASTNode> Function::parse_impl(  // NOLINT
    AST::ASTContext& context,                        // NOLINT
    std::list<Lexical::Token>& tokens,               // NOLINT
    const std::list<Lexical::Token>& attributes) {   // NOLINT
    auto func_name = pop(tokens).Value();

    /**
     * Parse Argument List
     */
    // consume '('
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses, tokens);

    // consume argument list
    AST::ArgumentList arguments;
    while (!tokens.empty() &&
           peek(tokens).Type() != Lexical::TokenType::kRParentheses) {
        // get base type
        auto [type, name_token] = ParseTypeDecl(context, tokens);
        if (type == nullptr) return nullptr;

        std::string name_str{name_token.Value()};
        if (MYCC_ExistInArgumentList(arguments, name_str)) {
            MYCC_PrintTokenError(name_token,
                                 "Duplicate argument name_token:" + name_str);
            return nullptr;
        }
        // consume ',' if we are not at the end
        if (peek(tokens).Type() != Lexical::TokenType::kRParentheses) {
            MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kComma, tokens);
        }
    }

    // check ')'  and consume
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses, tokens);

    // construct function node
    auto func_node =
        std::make_unique<AST::FunctionNode>(arguments, func_name, attributes);

    // trying to parse the function body if it exists
    if (peek(tokens).Type() == Lexical::TokenType::kLBrace) {
        // create a new named scope
        context.enterScope(func_name);

        // trying to parser the function body
        auto func_body = ParserFactory::ParseAST<AST::BlockStatement>(
            context, tokens, attributes);

        // if function body is syntax correct, then add it to the function
        // node and back to current the scope
        if (func_body) {
            func_node->set_body(std::move(func_body));
            context.leaveScope();
        } else {
            return nullptr;
        }
    }

    // if the function body is not exits, then we need to check the ";"
    else if (peek(tokens).Type() == Lexical::TokenType::kSemiColon) {
        pop(tokens);
    }

    // if the function body is not exits, then we need to check the ";"
    else {
        MYCC_PrintFirstTokenError_ReturnNull(
            tokens, "Except an \";\" but got:" + error_token.Value());
    }

    return std::move(func_node);
}
}  // namespace Mycc::Syntax::Parser