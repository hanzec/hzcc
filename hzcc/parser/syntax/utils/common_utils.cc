//
// Created by chen_ on 2022/3/24.
//

#include "common_utils.h"
namespace hzcc::syntax::utils {
using namespace hzcc::parser_common;
std::string TokenListToString(std::list<Token>& tokens) {
    if (tokens.empty()) return "";

    std::string str;
    for (auto& token : tokens) {
        str += token.Value() + " ";
    }
    return str.replace(str.end() - 1, str.end(), "");
}

std::unique_ptr<ast::Stmt> ParseCondition(TokenList& tokens, Ctx& context) {
    // next token is (
    HZCC_CheckAndConsume_ReturnErr(TokenType::kLParentheses, tokens)

        // parse condition
        auto condition = ParserFactory::ParseAST<ast::Stmt>(tokens, context);
    if (!condition) {
        DLOG(ERROR) << "Parse condition failed";
        return nullptr;
    }

    // next token is ')'
    HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens)

        return condition;
}

std::unique_ptr<ast::Stmt> ParseBodyStatement(TokenList& tokens, Ctx& context,
                                              bool add_semicolon) {
    if (tokens.peek().Type() == TokenType::kLBrace) {
        // enter new scope
        context.enter_scope();

        auto body_statement =
            ParserFactory::ParseAST<ast::CompoundStmt>(tokens, context);
        if (!body_statement) {
            DLOG(ERROR) << "Parse body stmt failed";
            return nullptr;
        }

        // leave scope
        context.leave_scope();

        if (add_semicolon) {
            // push a semicolon for easier parsing
            tokens.push(Token(TokenType::kSemiColon, -1, -1));
        }

        return body_statement;
    } else {
        auto body_statement =
            ParserFactory::ParseAST<ast::Stmt>(tokens, context);

        if (!body_statement) {
            DLOG(ERROR) << "Parse body stmt failed";
            return nullptr;
        }

        if (!body_statement->HasBody()) {
            // consume ';'
            HZCC_CheckAndConsume_ReturnErr(TokenType::kSemiColon, tokens)
        }

        return body_statement;
    }
}

std::tuple<Token, std::list<std::unique_ptr<ast::Stmt>>>
ParseVariable(     // NOLINT
    Ctx& context,  // NOLINT
    TokenList& tokens) {
    auto variable_name = tokens.pop();
    std::list<std::unique_ptr<ast::Stmt>> array_shape;

    /**
     * Parse array type
     */
    if (tokens.peek().Type() == TokenType::kLBracket) {
        // for array without size we are only allowed maximum of 1 level
        if (tokens.size() >= 2 &&
            tokens.peek2().Type() == TokenType::kRBracket) {
            tokens.pop();  // consume "]"
            tokens.pop();  // consume "["
            array_shape.push_back(nullptr);
            return std::make_tuple(variable_name, std::move(array_shape));
        }

        // for array with have multiple dimension, it must have a clear
        // definition of its shape
        while (tokens.peek().Type() == TokenType::kLBracket) {
            if (tokens.size() >= 2 &&
                tokens.peek2().Type() != TokenType::kRBracket) {
                tokens.pop();  // consume "["

                // we could have an array size here
                auto array_size =
                    ParserFactory::ParseAST<ast::Stmt>(tokens, context);
                if (array_size == nullptr)
                    return std::make_tuple(Token(TokenType::kUnknown, -1, -1),
                                           std::move(array_shape));

                // consume "]"
                if (!checkAndPrint(TokenType::kRBracket, tokens)) {
                    return std::make_tuple(Token(TokenType::kUnknown, -1, -1),
                                           std::move(array_shape));
                } else {
                    tokens.pop();  // consume "]"
                    array_shape.push_back(std::move(array_size));
                }
            } else {
                return syntax::utils::PrintTokenErr(
                    tokens, "have incomplete type") return std::
                    make_tuple(Token(TokenType::kUnknown, -1, -1),
                               std::move(array_shape));
            }
        }

        // return final variable and its shape
        return std::make_tuple(variable_name, std::move(array_shape));
    } else {
        return std::make_tuple(variable_name, std::move(array_shape));
    }
}

std::tuple<std::shared_ptr<ast::Type>, Token> ParseTypeDecl(
    const std::string& type_name, TokenList& tokens, Ctx& context) {
    /**
     * Parse name of variable
     */
    if (tokens.peek().Type() == TokenType::kIdentity) {
        auto [variable_name, array_shape] = ParseVariable(context, tokens);

        // check if variable name is valid
        if (variable_name.Type() == TokenType::kUnknown) {
            return std::make_tuple(nullptr, Token(TokenType::kUnknown, -1, -1));
        }

        // check if current type is array
        if (!array_shape.empty()) {
            auto attrs = tokens.cache_attr_list();
            auto context_type = context.get_type(type_name, {});
            return std::make_tuple(
                context.getArrayType(context_type, attrs, array_shape),
                variable_name);
        } else {
            auto attrs = tokens.cache_attr_list();
            return std::make_tuple(context.get_type(type_name, attrs),
                                   variable_name);
        }
    }

    /**
     * Parse Function Pointer retType
     */
    else if (tokens.peek().Type() == TokenType::kLParentheses) {
        tokens.pop();

        /**
         * parser Function Pointer name
         */
        // consume "*"
        if (!checkAndPrint(TokenType::kReference, tokens)) {
            return std::make_tuple(nullptr, Token(TokenType::kUnknown, -1, -1));
        }

        // get function Pointer name
        if (!checkAndPrint(TokenType::kIdentity, tokens)) {
            return std::make_tuple(nullptr, Token(TokenType::kUnknown, -1, -1));
        }
        auto name = tokens.pop();

        // consume ")"
        if (!checkAndPrint(TokenType::kRParentheses, tokens)) {
            return std::make_tuple(nullptr, Token(TokenType::kUnknown, -1, -1));
        }

        /**
         * parser Function Pointer ArgumentList
         */
        // consume "("
        if (!checkAndPrint(TokenType::kLParentheses, tokens)) {
            return std::make_tuple(nullptr, Token(TokenType::kUnknown, -1, -1));
        }

        // consume function Pointer argument list
        std::list<std::shared_ptr<ast::Type>> func_arguments;
        while (tokens.peek().Type() != TokenType::kRParentheses) {
            // token list should not be empty
            if (tokens.size() == 1) {
                return syntax::utils::PrintTokenErr(
                    tokens, "Except an ) but got nothing") return std::
                    make_tuple(nullptr, Token(TokenType::kUnknown, -1, -1));
            }

            // get base type
            auto types = ParseTypeName(tokens, context);
            auto func_args_type =
                ParseTypeDecl(TokenListToString(types), tokens, context);
            if (std::get<0>(func_args_type) == nullptr)
                return std::make_tuple(nullptr,
                                       Token(TokenType::kUnknown, -1, -1));

            func_arguments.push_back(std::get<0>(func_args_type));

            // consume ","
            if (tokens.peek().Type() != TokenType::kRParentheses) {
                if (!checkAndPrint(TokenType::kComma, tokens)) {
                    return std::make_tuple(nullptr,
                                           Token(TokenType::kUnknown, -1, -1));
                }
            }
        }

        // consume ")"
        if (!checkAndPrint(TokenType::kRParentheses, tokens)) {
            return std::make_tuple(nullptr, Token(TokenType::kUnknown, -1, -1));
        }

        return std::make_tuple(context.getFuncPtrType(type_name), name);
    }

    /**
     * We are allowed no identifier after type
     */
    else {
        if (context.hasType(type_name)) {
            auto attrs = tokens.cache_attr_list();
            return std::make_tuple(context.get_type(type_name, attrs),
                                   Token(TokenType::kUnknown, -1, -1));
        } else {
            // TODO we need print more useful error message
            DLOG(WARNING) << "retType " << type_name << " is not defined";
            return std::make_tuple(nullptr, Token(TokenType::kUnknown, -1, -1));
        }
    }
}
}  // namespace hzcc::syntax::utils
