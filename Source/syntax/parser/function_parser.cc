//
// Created by chen_ on 2022/3/24.
//
#include "function_parser.h"

#include <list>

#include "AST/CompilationUnit.h"
#include "AST/stmt/CompoundStmt.h"
#include "AST/stmt/FuncDeclStmt.h"
#include "lexical/Token.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"

namespace Hzcc::Syntax::Parser {
Function::Function() noexcept
    : ParserBase(TypeNameUtil::hash<AST::FuncDeclStmt>(),
                 TypeNameUtil::name_pretty<AST::FuncDeclStmt>()) {}

std::unique_ptr<AST::ASTNode> Function::parse_impl(TokenList& tokens,
                                                   SyntaxContext& context) {
    auto return_type_name_str = ParseTypeName(tokens, context);

    if (return_type_name_str.empty() ||
        !context.hasType(TokenListToString(return_type_name_str))) {
        return nullptr;
    }

    auto return_type =
        context.getNamedType(TokenListToString(return_type_name_str), {});

    auto func_name = tokens.pop();

    // function can only be defined in global scope
    if (!context.AtRoot()) {
        MYCC_PrintTokenError_ReturnNull(
            func_name, "function can only be defined in global scope");
    }

    /**
     * Parse Argument List
     */
    // consume '('
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses, tokens);

    // next token should not be ',', if it is, it means empty argument list
    if (tokens.peek().Type() == Lexical::TokenType::kComma) {
        MYCC_PrintFirstTokenError_ReturnNull(tokens, "Expected type name");
    }

    // function name cannot be empty
    if (!Options::Global_allow_same_name_for_func_val &&
        context.hasVariable(func_name.Value(), true)) {
        MYCC_PrintTokenError_ReturnNull(
            func_name, "Function '" + func_name.Value() +
                           "' already defined as global variable");
    }

    // actual function node
    auto func_node = std::make_unique<AST::FuncDeclStmt>(
        func_name.Location(), func_name.Value(), return_type);

    // parse argument list
    bool first = false;
    std::list<std::shared_ptr<AST::Type>> arg_types;
    do {
        // consume ',' if available
        if (tokens.peek().Type() == Lexical::TokenType::kComma) {
            tokens.pop();
        } else {
            // directly break if ')'
            if (tokens.peek().Type() == Lexical::TokenType::kRParentheses) {
                break;
            } else if (first) {
                MYCC_PrintFirstTokenError_ReturnNull(tokens, "Expected ','");
            }
        }

        // get base argument_type
        auto head = tokens.peek();
        auto type_name = ParseTypeName(tokens, context);

        if (type_name.empty()) {
            MYCC_PrintTokenError(tokens.peek(), "Expected type name");
            return nullptr;
        }

        auto [argument_type, name_token] =
            ParseTypeDecl(TokenListToString(type_name), tokens, context);

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
        auto attr = tokens.LoadCachedAttributes();
        func_node->AddFunctionArgument(std::make_unique<AST::ParamVarDecl>(
            argument_type, name_token.Value(), name_token.Location()));
    } while (!tokens.empty() &&
             tokens.peek().Type() != Lexical::TokenType::kRParentheses);

    // check ')'  and consume
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses, tokens);

    // function with same name must match previous definition
    if (context.hasFunction(func_name.Value())) {
        auto [func_type, func_type_attributes, line_no] =
            context.getFuncRetAndArgType(func_name.Value());

        // compare argument number
        if (func_type_attributes.size() != arg_types.size()) {
            MYCC_PrintTokenError_ReturnNull(
                func_name, "Function '" + func_name.Value() +
                               "' already defined with different argument "
                               "number from previous definition in line" +
                               std::to_string(line_no));
        }

        // compare return type
        if (!(*func_type == *return_type)) {
            MYCC_PrintTokenError_ReturnNull(
                func_name, "Function " + return_type->GetName() + " " +
                               func_name.Value() + " already defined with " +
                               func_type->GetName() + " return type in line " +
                               std::to_string(line_no));
        }

        // compare all argument type
        for (const auto& type : arg_types) {
            if (!(*type == *func_type_attributes.front())) {
                MYCC_PrintTokenError_ReturnNull(
                    func_name, "Function " + func_name.Value() +
                                   " already defined with different argument "
                                   "type in line " +
                                   std::to_string(line_no));
            }
            func_type_attributes.pop_front();
        }
    }

    // add function definition to context
    context.addFunction(func_name.Location().first, func_name.Value(),
                        return_type, arg_types);

    // trying to parse the function body if it exists
    if (tokens.peek().Type() == Lexical::TokenType::kLBrace) {
        // create a new named scope
        context.enterScope(func_name.Value(), return_type);

        // add all arguments to the scope
        for (auto& arg : func_node->getArguments()) {
            context.addVariable(std::get<2>(arg), std::get<0>(arg),
                                std::get<1>(arg));
        }

        // trying to parser the function body
        auto func_body =
            ParserFactory::ParseAST<AST::CompoundStmt>(tokens, context);

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
    else if (tokens.peek().Type() == Lexical::TokenType::kSemiColon) {
        tokens.pop();
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
}  // namespace Hzcc::Syntax::Parser
