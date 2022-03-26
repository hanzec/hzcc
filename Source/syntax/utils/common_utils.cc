//
// Created by chen_ on 2022/3/24.
//

#include "common_utils.h"

#include "AST/ast_context.h"
#include "AST/statement/block_parser.h"
#include "lexical/Token.h"
#include "lexical/token_type.h"
#include "syntax/Parser.h"
#include "token_list_utils.h"
#include "utils/message_utils.h"
namespace Mycc::Syntax::Parser {
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
}

std::unique_ptr<AST::ASTNode> ParseBodyStatement(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    if (TokenListUtils::peek(tokens).Type() == Lexical::TokenType::kLBrace) {
        return ParserFactory::ParseAST<AST::BlockStatement>(context, tokens);
    } else {
        return ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
    }
}

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

std::tuple<std::shared_ptr<AST::Type>, Lexical::Token> ParseTypeDecl(  // NOLINT
    AST::ASTContext& context,                                          // NOLINT
    std::list<Lexical::Token>& tokens) {                               // NOLINT
    std::string final_type_name;

    /**
     * Parse type name
     */
    // struct or union
    if (peek(tokens).Type() == Lexical::TokenType::kStruct ||
        peek(tokens).Type() == Lexical::TokenType::kUnion) {
        final_type_name += pop(tokens).Value() + " ";
    }

    // type name
    if (peek(tokens).Type() == Lexical::TokenType::kIdentity ||
        peek(tokens).Type() == Lexical::TokenType::kType) {
        final_type_name += pop(tokens).Value();
    }

    // type name also could have reference and dereference
    // check if it is reference and dereference
    if (peek(tokens).Type() == Lexical::TokenType::kReference ||
        peek(tokens).Type() == Lexical::TokenType::kDereference) {
        final_type_name += pop(tokens).Value();
        auto ref_type = pop(tokens).Type();

        // dereference more than twice is not allowed
        if (peek(tokens).Type() == Lexical::TokenType::kDereference) {
            final_type_name += pop(tokens).Value();
            if (peek(tokens).Type() == Lexical::TokenType::kDereference) {
                MYCC_PrintFirstTokenError(
                    tokens, "Dereference more than twice is not allowed");
                return std::make_tuple(
                    nullptr,
                    Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
            } else if (peek(tokens).Type() == Lexical::TokenType::kReference) {
                MYCC_PrintFirstTokenError(
                    tokens, "Reference and dereference can not be mixed");
                return std::make_tuple(
                    nullptr,
                    Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
            }
        }

        while (peek(tokens).Type() == Lexical::TokenType::kReference) {
            // consume "*"
            if (!checkAndPrint(ref_type, tokens)) {
                return std::make_tuple(
                    nullptr,
                    Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
            } else {
                final_type_name += pop(tokens).Value();
            }
        }
    }

    /**
     * Parse name of variable
     */
    if (peek(tokens).Type() == Lexical::TokenType::kIdentity) {
        auto type_name = pop(tokens);

        /**
         * Parse array type
         */
        if (peek(tokens).Type() == Lexical::TokenType::kLBracket) {
            // for array without size we are only allowed maximum of 1 level
            if (peek2(tokens).Type() == Lexical::TokenType::kRBracket) {
                pop(tokens);  // consume "]"
                pop(tokens);  // consume "["
                return std::make_tuple(context.getArrayType(final_type_name),
                                       type_name);
            }

            // for array with have multiple dimension, it must have a clear
            // definition of its shape
            std::list<std::unique_ptr<AST::ASTNode>> array_shape;
            while (peek(tokens).Type() != Lexical::TokenType::kLBracket) {
                if (peek2(tokens).Type() != Lexical::TokenType::kRBracket) {
                    pop(tokens);  // consume "]"

                    // we could have an array size here
                    auto array_size =
                        ParserFactory::ParseAST<AST::ASTNode>(context, tokens);
                    if (array_size == nullptr)
                        return std::make_tuple(
                            nullptr, Lexical::Token(
                                         Lexical::TokenType::kUnknown, -1, -1));

                    // consume "]"
                    if (!checkAndPrint(Lexical::TokenType::kRBracket, tokens)) {
                        return std::make_tuple(
                            nullptr, Lexical::Token(
                                         Lexical::TokenType::kUnknown, -1, -1));
                    } else {
                        pop(tokens);  // consume "]"
                        array_shape.push_back(std::move(array_size));
                    }
                } else {
                    MYCC_PrintFirstTokenError(tokens,
                                              "have incomplete type of " +
                                                  final_type_name + "[]");
                    return std::make_tuple(
                        nullptr,
                        Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
                }
            }

            // return final array type
            if(context.hasType(final_type_name)) {
                return std::make_tuple(
                    context.getArrayType(final_type_name, array_shape),
                    type_name);
            } else {
                MYCC_PrintFirstTokenError(
                    tokens, "type " + final_type_name + " is not defined");
                return std::make_tuple(
                    nullptr,
                    Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
            }
        } else {
            return std::make_tuple(context.getType(final_type_name), type_name);
        }
    }

    /**
     * Parse Function Pointer Type
     */
    else if (peek(tokens).Type() == Lexical::TokenType::kLParentheses) {
        pop(tokens);

        /**
         * Parser Function Pointer name
         */
        // consume "*"
        if (!checkAndPrint(Lexical::TokenType::kReference, tokens)) {
            return std::make_tuple(
                nullptr, Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }

        // get function Pointer name
        if (!checkAndPrint(Lexical::TokenType::kIdentity, tokens)) {
            return std::make_tuple(
                nullptr, Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }
        auto name = pop(tokens);

        // consume ")"
        if (!checkAndPrint(Lexical::TokenType::kRParentheses, tokens)) {
            return std::make_tuple(
                nullptr, Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }

        /**
         * Parser Function Pointer ArgumentList
         */
        // consume "("
        if (!checkAndPrint(Lexical::TokenType::kLParentheses, tokens)) {
            return std::make_tuple(
                nullptr, Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }

        // consume function Pointer argument list
        std::list<std::shared_ptr<AST::Type>> func_arguments;
        while (peek(tokens).Type() != Lexical::TokenType::kRParentheses) {
            // token list should not be empty
            if (tokens.size() == 1) {
                MYCC_PrintFirstTokenError(
                    tokens, "Except an ) but got nothing") return std::
                    make_tuple(
                        nullptr,
                        Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
            }

            // get base type
            auto func_args_type = ParseTypeDecl(context, tokens);
            if (std::get<0>(func_args_type) == nullptr)
                return std::make_tuple(
                    nullptr,
                    Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));

            func_arguments.push_back(std::get<0>(func_args_type));

            // consume ","
            if (peek(tokens).Type() != Lexical::TokenType::kRParentheses) {
                if (!checkAndPrint(Lexical::TokenType::kComma, tokens)) {
                    return std::make_tuple(
                        nullptr,
                        Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
                }
            }
        }

        // consume ")"
        if (!checkAndPrint(Lexical::TokenType::kRParentheses, tokens)) {
            return std::make_tuple(
                nullptr, Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }

        return std::make_tuple(
            context.getFuncPtrType(final_type_name, func_arguments), name);
    }

    /**
     * We are allowed no identifier after type
     */
    else {
        if (context.hasType(final_type_name)) {
            return std::make_tuple(
                context.getType(final_type_name),
                Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        } else {
            // TODO we need print more useful error message
            DLOG(ERROR) << "Type " << final_type_name << " is not defined";
            return std::make_tuple(
                nullptr, Lexical::Token(Lexical::TokenType::kUnknown, -1, -1));
        }
    }
}
}  // namespace Mycc::Syntax::Parser
