//
// Created by chen_ on 2022/3/24.
//
#include "function_parser.h"

#include <list>

#include "AST/ASTContext.h"
#include "AST/statement/compound.h"
#include "AST/statement/function_decl.h"
#include "lexical/Token.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"

namespace Mycc::Syntax::Parser {
using namespace TokenListUtils;
Function::Function() noexcept
    : ParserBase(TypeNameUtil::hash<AST::FunctionDeclNode>(),
                 TypeNameUtil::name_pretty<AST::FunctionDeclNode>()) {}

std::unique_ptr<AST::ASTNode> Function::parse_impl(AST::ASTContext& context,
                                                   TokenList& tokens,
                                                   TokenList& attributes) {
    ConcatAttribute(attributes, tokens);
    auto return_type_name_str = ParseTypeName(context, tokens);

    if (return_type_name_str.empty() ||
        !context.hasType(TokenListToString(return_type_name_str))) {
        return nullptr;
    }

    auto return_type =
        context.getType(TokenListToString(return_type_name_str), {});

    auto func_name = pop_list(tokens);

    /**
     * Parse Argument List
     */
    // consume '('
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses, tokens);

    // next token should not be ',', if it is, it means empty argument list
    if (tokens.front().Type() == Lexical::TokenType::kComma) {
        MYCC_PrintFirstTokenError_ReturnNull(tokens, "Expected type name");
    }

    // function name cannot be empty
    if (Options::Global_enable_type_checking &&
        context.hasVariable(func_name.Value())) {
        MYCC_PrintTokenError_ReturnNull(
            func_name, "Function '" + func_name.Value() +
                           "' already defined as global variable");
    }

    // actual function node
    auto func_node = std::make_unique<AST::FunctionDeclNode>(
        func_name, return_type, attributes);

    // parse argument list
    bool first = false;
    std::list<std::shared_ptr<AST::Type>> arg_types;
    do {
        // consume ',' if available
        if (peek(tokens).Type() == Lexical::TokenType::kComma) {
            pop_list(tokens);
        } else {
            // directly break if ')'
            if (peek(tokens).Type() == Lexical::TokenType::kRParentheses) {
                break;
            } else if (first) {
                MYCC_PrintFirstTokenError_ReturnNull(tokens, "Expected ','");
            }
        }

        // get base argument_type
        auto head = tokens.front();
        auto type_name = ParseTypeName(context, tokens);

        if (type_name.empty()) {
            MYCC_PrintTokenError(peek(tokens), "Expected type name");
            return nullptr;
        }

        auto [argument_type, attrs, name_token] =
            ParseTypeDecl(TokenListToString(type_name), context, tokens);

        if (argument_type == nullptr) {
            if (head == *tokens.begin()) {
                MYCC_PrintTokenError(head, "Expected type name");
            } else if (name_token.Type() != Lexical::kUnknown) {
                MYCC_PrintTokenError(name_token, "Expected type name");
            }
            return nullptr;
        }

        // check if attribute are duplicated
        for (auto& attr_1 : type_name) {
            for (auto& attr_2 : type_name) {
                if ((!(attr_1 == attr_2)) && attr_1.Value() == attr_2.Value()) {
                    MYCC_PrintTokenError_ReturnNull(attr_1,
                                                    "Duplicated attribute");
                }
            }
        }

        first = true;
        arg_types.push_back(argument_type);
        func_node->AddFunctionArgument(std::make_unique<AST::ParamVarDecl>(
            name_token, argument_type, attrs));
    } while (!tokens.empty() &&
             peek(tokens).Type() != Lexical::TokenType::kRParentheses);

    // check ')'  and consume
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses, tokens);

    // add function definition to context
    context.addFunction(func_name.Value(), return_type, arg_types);

    // trying to parse the function body if it exists
    if (peek(tokens).Type() == Lexical::TokenType::kLBrace) {
        // create a new named scope
        context.enterScope(func_name.Value(), return_type);

        // add all arguments to the scope
        for (auto& arg : func_node->getArguments()) {
            context.addVariable(arg.first, arg.second);
        }

        // trying to parser the function body
        auto func_body = ParserFactory::ParseAST<AST::CompoundStmt>(
            context, tokens, attributes);

        // if function body is syntax correct, then add it to the function
        // node and back to current the scope
        if (func_body) {
            func_node->set_body(std::move(func_body));
            context.leaveScope();
        } else {
            context.leaveScope();
            return nullptr;
        }
    }

    // if the function body is not exits, then we need to check the ";"
    else if (peek(tokens).Type() == Lexical::TokenType::kSemiColon) {
        pop_list(tokens);
    }

    // if the function body is not exits, then we need to check the ";"
    else {
        MYCC_PrintFirstTokenError_ReturnNull(
            tokens, "Except an \";\" but got:" + error_token.Value());
    }

    // check function name is redefinition or not
    if (func_node->HasBody() && context.hasFunction(func_name.Value()) &&
        context.hasFunctionBody(func_name.Value())) {
        MYCC_PrintTokenError_ReturnNull(
            func_name, "Function '" + func_name.Value() + "' already defined");
    }

    return std::move(func_node);
}
}  // namespace Mycc::Syntax::Parser
