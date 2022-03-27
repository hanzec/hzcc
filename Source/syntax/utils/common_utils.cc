//
// Created by chen_ on 2022/3/24.
//

#include "common_utils.h"

#include "AST/ast_context.h"
#include "AST/statement/compound.h"
#include "lexical/Token.h"
#include "lexical/token_type.h"
#include "syntax/Parser.h"
#include "token_list_utils.h"
#include "utils/message_utils.h"
namespace Mycc::Syntax::Parser {
ALWAYS_INLINE static bool checkAndPrint(
    Lexical::TokenType type, const std::list<Lexical::Token>& tokens) {
    if (tokens.front().Type() != type) {
        MYCC_PrintFirstTokenError(
            tokens,
            "Except an " +
                Lexical::SymbolUtils::TokenTypeToString(tokens.front().Type()) +
                " but got: " +
                ((int)error_token.Type() <= 0xFF
                     ? "[Symbol"
                     : "[" + Lexical::SymbolUtils::TokenTypeToString(
                                 error_token.Type())) +
                "]:@" + error_token.Value() + "@");
        return false;
    } else {
        return true;
    }
}

std::string TokenListToString(std::list<Lexical::Token>& tokens) {
    if (tokens.empty()) return "";

    std::string str;
    for (auto& token : tokens) {
        str += token.Value() + " ";
    }
    return str.replace(str.end() - 1, str.end(), "");
}

std::list<Lexical::Token> ParseTypeName(AST::ASTContext& context,
                                        std::list<Lexical::Token>& tokens) {
    auto token = tokens.front();
    std::list<Lexical::Token> final_type_name;

    // append attrs to final_type_name
    while (Lexical::TokenUtils::IsAttribute(tokens.front().Type())) {
        final_type_name.push_back(pop_list(tokens));
    }

    // struct or union
    if (peek(tokens).Type() == Lexical::TokenType::kStruct ||
        peek(tokens).Type() == Lexical::TokenType::kUnion) {
        final_type_name.push_back(pop_list(tokens));

        // struct or union name
        if (peek(tokens).Type() != Lexical::TokenType::kIdentity ||
            context.hasType(peek(tokens).Value())) {
            MYCC_PrintTokenError(peek(tokens), "except a identifier") return {};
        } else {
            final_type_name.push_back(pop_list(tokens));
        }
    }

    // type name
    if ((peek(tokens).Type() == Lexical::TokenType::kIdentity &&
         context.hasType(peek(tokens).Value())) ||
        peek(tokens).Type() == Lexical::TokenType::kType) {
        final_type_name.push_back(pop_list(tokens));
    }

    // type name also could have reference and dereference
    // check if it is reference and dereference
    if (peek(tokens).Type() == Lexical::TokenType::kReference ||
        peek(tokens).Type() == Lexical::TokenType::kDereference) {
        final_type_name.push_back(pop_list(tokens));
        auto ref_type = pop_list(tokens).Type();

        // dereference more than twice is not allowed
        if (peek(tokens).Type() == Lexical::TokenType::kDereference) {
            final_type_name.push_back(pop_list(tokens));
            if (peek(tokens).Type() == Lexical::TokenType::kDereference) {
                MYCC_PrintFirstTokenError(
                    tokens, "Dereference more than twice is not allowed");
                return {};
            } else if (peek(tokens).Type() == Lexical::TokenType::kReference) {
                MYCC_PrintFirstTokenError(
                    tokens, "Reference and dereference can not be mixed");
                return {};
            }
        }

        while (peek(tokens).Type() == Lexical::TokenType::kReference) {
            // consume "*"
            if (!checkAndPrint(ref_type, tokens)) {
                return {};
            } else {
                final_type_name.push_back(pop_list(tokens));
            }
        }
    }

    return final_type_name;
}

std::unique_ptr<AST::ASTNode> ParseCondition(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    // next token is (
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses, tokens);

    // parse condition
    auto condition = ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
    if (!condition) {
        DLOG(ERROR) << "Parse condition failed";
        return nullptr;
    }

    // next token is )
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses, tokens);

    return condition;
}

std::unique_ptr<AST::ASTNode> ParseBodyStatement(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens,
    bool add_semicolon) {
    if (TokenListUtils::peek(tokens).Type() == Lexical::TokenType::kLBrace) {
        // enter new scope
        context.enterScope();

        auto body_statement =
            ParserFactory::ParseAST<AST::CompoundStmt>(context, tokens);
        if (!body_statement) {
            DLOG(ERROR) << "Parse body statement failed";
            return nullptr;
        }

        // leave scope
        context.leaveScope();

        if (add_semicolon) {
            // push a semicolon for easier parsing
            tokens.push_front(
                Lexical::Token(Lexical::TokenType::kSemiColon, -1, -1));
        }

        return body_statement;
    } else {
        auto body_statement =
            ParserFactory::ParseAST<AST::ASTNode>(context, tokens);

        if (!body_statement) {
            DLOG(ERROR) << "Parse body statement failed";
            return nullptr;
        }

        if (!body_statement->HasBody()) {
            // consume ';'
            MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kSemiColon,
                                            tokens);
        }

        return body_statement;
    }
}

std::tuple<Lexical::Token, std::list<std::unique_ptr<AST::ASTNode>>>
ParseVariable(                 // NOLINT
    AST::ASTContext& context,  // NOLINT
    std::list<Lexical::Token>& tokens) {
    auto variable_name = pop_list(tokens);
    std::list<std::unique_ptr<AST::ASTNode>> array_shape;

    /**
     * Parse array type
     */
    if (peek(tokens).Type() == Lexical::TokenType::kLBracket) {
        // for array without size we are only allowed maximum of 1 level
        if (tokens.size() >= 2 &&
            peek2(tokens).Type() == Lexical::TokenType::kRBracket) {
            pop_list(tokens);  // consume "]"
            pop_list(tokens);  // consume "["
            array_shape.push_back(nullptr);
            return std::make_tuple(variable_name, std::move(array_shape));
        }

        // for array with have multiple dimension, it must have a clear
        // definition of its shape
        while (peek(tokens).Type() == Lexical::TokenType::kLBracket) {
            if (tokens.size() >= 2 &&
                peek2(tokens).Type() != Lexical::TokenType::kRBracket) {
                pop_list(tokens);  // consume "["

                // we could have an array size here
                auto array_size =
                    ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
                if (array_size == nullptr)
                    return std::make_tuple(
                        Lexical::Token(Lexical::TokenType::kUnknown, -1, -1),
                        std::move(array_shape));

                // consume "]"
                if (!checkAndPrint(Lexical::TokenType::kRBracket, tokens)) {
                    return std::make_tuple(
                        Lexical::Token(Lexical::TokenType::kUnknown, -1, -1),
                        std::move(array_shape));
                } else {
                    pop_list(tokens);  // consume "]"
                    array_shape.push_back(std::move(array_size));
                }
            } else {
                MYCC_PrintFirstTokenError(tokens, "have incomplete type");
                return std::make_tuple(
                    Lexical::Token(Lexical::TokenType::kUnknown, -1, -1),
                    std::move(array_shape));
            }
        }

        // return final variable and its shape
        return std::make_tuple(variable_name, std::move(array_shape));
    } else {
        return std::make_tuple(variable_name, std::move(array_shape));
    }
}

std::tuple<std::shared_ptr<AST::Type>, TokenList, Lexical::Token>
ParseTypeDecl(                            // NOLINT
    std::string type_name,                // NOLINT
    AST::ASTContext& context,             // NOLINT
    std::list<Lexical::Token>& tokens) {  // NOLINT
    auto attrs = GetAttribute(tokens);

    /**
     * Parse name of variable
     */
    if (peek(tokens).Type() == Lexical::TokenType::kIdentity) {
        auto [variable_name, array_shape] = ParseVariable(context, tokens);

        // check if variable name is valid
        if (variable_name.Type() == Lexical::TokenType::kUnknown) {
            return std::make_tuple(
                nullptr, TokenList(),
                Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }

        // check if current type is array
        if (!array_shape.empty()) {
            auto context_type = context.getType(type_name);
            return std::make_tuple(context.getType(context_type, array_shape),
                                   attrs, variable_name);
        } else {
            return std::make_tuple(context.getType(type_name), attrs,
                                   variable_name);
        }
    }

    /**
     * Parse Function Pointer Type
     */
    else if (peek(tokens).Type() == Lexical::TokenType::kLParentheses) {
        pop_list(tokens);

        /**
         * Parser Function Pointer name
         */
        // consume "*"
        if (!checkAndPrint(Lexical::TokenType::kReference, tokens)) {
            return std::make_tuple(
                nullptr, TokenList(),
                Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }

        // get function Pointer name
        if (!checkAndPrint(Lexical::TokenType::kIdentity, tokens)) {
            return std::make_tuple(
                nullptr, TokenList(),
                Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }
        auto name = pop_list(tokens);

        // consume ")"
        if (!checkAndPrint(Lexical::TokenType::kRParentheses, tokens)) {
            return std::make_tuple(
                nullptr, TokenList(),
                Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }

        /**
         * Parser Function Pointer ArgumentList
         */
        // consume "("
        if (!checkAndPrint(Lexical::TokenType::kLParentheses, tokens)) {
            return std::make_tuple(
                nullptr, TokenList(),
                Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }

        // consume function Pointer argument list
        std::list<std::shared_ptr<AST::Type>> func_arguments;
        while (peek(tokens).Type() != Lexical::TokenType::kRParentheses) {
            // token list should not be empty
            if (tokens.size() == 1) {
                MYCC_PrintFirstTokenError(
                    tokens, "Except an ) but got nothing") return std::
                    make_tuple(
                        nullptr, TokenList(),
                        Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
            }

            // get base type
            auto types = ParseTypeName(context, tokens);
            auto func_args_type =
                ParseTypeDecl(TokenListToString(types), context, tokens);
            if (std::get<0>(func_args_type) == nullptr)
                return std::make_tuple(
                    nullptr, TokenList(),
                    Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));

            func_arguments.push_back(std::get<0>(func_args_type));

            // consume ","
            if (peek(tokens).Type() != Lexical::TokenType::kRParentheses) {
                if (!checkAndPrint(Lexical::TokenType::kComma, tokens)) {
                    return std::make_tuple(
                        nullptr, TokenList(),
                        Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
                }
            }
        }

        // consume ")"
        if (!checkAndPrint(Lexical::TokenType::kRParentheses, tokens)) {
            return std::make_tuple(
                nullptr, TokenList(),
                Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }

        return std::make_tuple(
            context.getType(context.getType(type_name), func_arguments), attrs,
            name);
    }

    /**
     * We are allowed no identifier after type
     */
    else {
        if (context.hasType(type_name)) {
            return std::make_tuple(
                context.getType(type_name), attrs,
                Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        } else {
            // TODO we need print more useful error message
            DLOG(WARNING) << "Type " << type_name << " is not defined";
            return std::make_tuple(
                nullptr, TokenList(),
                Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }
    }
}
}  // namespace Mycc::Syntax::Parser
