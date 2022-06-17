//
// Created by chen_ on 2022/3/24.
//

#include "common_utils.h"

#include "AST/CompilationUnit.h"
#include "AST/stmt/CompoundStmt.h"
#include "lexical/Token.h"
#include "syntax/Parser.h"
#include "token_type.h"
#include "utils/keywords_utils.h"
#include "utils/message_utils.h"
namespace Hzcc::Syntax::Parser {
ALWAYS_INLINE static bool checkAndPrint(TokenType type, TokenList& tokens) {
    if (tokens.peek().Type() != type) {
        MYCC_PrintFirstTokenError(
            tokens, "Except an " +
                        KeywordsUtils::TokenTypeToString(tokens.peek().Type()) +
                        " but got: " +
                        ((int)error_token.Type() <= 0xFF
                             ? "[Symbol"
                             : "[" + KeywordsUtils::TokenTypeToString(
                                         error_token.Type())) +
                        "]:@" + error_token.Value() + "@") return false;
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

std::list<Lexical::Token> ParseTypeName(TokenList& tokens,
                                        SyntaxContext& context) {
    bool is_struct = false;
    std::list<Lexical::Token> final_type_name;

    // struct or union
    if (tokens.peek().Type() == TokenType::kStruct ||
        tokens.peek().Type() == TokenType::kUnion) {
        is_struct = true;
        final_type_name.push_back(tokens.pop());

        // struct or union name
        if (tokens.peek().Type() != TokenType::kIdentity ||
            context.hasType(tokens.peek().Value())) {
            MYCC_PrintTokenError(tokens.peek(),
                                 "except a identifier") return {};
        } else {
            final_type_name.push_back(tokens.pop());
        }
    }

    // type name
    if ((tokens.peek().Type() == TokenType::kIdentity &&
         context.hasType(tokens.peek().Value())) ||
        tokens.peek().Type() == TokenType::kType) {
        final_type_name.push_back(tokens.pop());
    } else if (!is_struct) {
        return {};
    }

    // type name also could have reference and dereference
    // check if it is reference and dereference
    if (tokens.peek().Type() == TokenType::kReference ||
        tokens.peek().Type() == TokenType::kDereference) {
        final_type_name.push_back(tokens.pop());
        auto ref_type = tokens.pop().Type();

        // dereference more than twice is not allowed
        if (tokens.peek().Type() == TokenType::kDereference) {
            final_type_name.push_back(tokens.pop());
            if (tokens.peek().Type() == TokenType::kDereference) {
                MYCC_PrintFirstTokenError(
                    tokens,
                    "Dereference more than twice is not allowed") return {};
            } else if (tokens.peek().Type() == TokenType::kReference) {
                MYCC_PrintFirstTokenError(
                    tokens,
                    "Reference and dereference can not be mixed") return {};
            }
        }

        while (tokens.peek().Type() == TokenType::kReference) {
            // consume "*"
            if (!checkAndPrint(ref_type, tokens)) {
                return {};
            } else {
                final_type_name.push_back(tokens.pop());
            }
        }
    }

    return final_type_name;
}

std::unique_ptr<AST::ASTNode> ParseCondition(TokenList& tokens,
                                             SyntaxContext& context) {
    // next token is (
    MYCC_CheckAndConsume_ReturnNull(TokenType::kLParentheses, tokens)

        // parse condition
        auto condition = ParserFactory::ParseAST<AST::ASTNode>(tokens, context);
    if (!condition) {
        DLOG(ERROR) << "Parse condition failed";
        return nullptr;
    }

    // next token is ')'
    MYCC_CheckAndConsume_ReturnNull(TokenType::kRParentheses, tokens)

        return condition;
}

std::unique_ptr<AST::ASTNode> ParseBodyStatement(TokenList& tokens,
                                                 SyntaxContext& context,
                                                 bool add_semicolon) {
    if (tokens.peek().Type() == TokenType::kLBrace) {
        // enter new scope
        context.enterScope();

        auto body_statement =
            ParserFactory::ParseAST<AST::CompoundStmt>(tokens, context);
        if (!body_statement) {
            DLOG(ERROR) << "Parse body stmt failed";
            return nullptr;
        }

        // leave scope
        context.leaveScope();

        if (add_semicolon) {
            // push a semicolon for easier parsing
            tokens.push(Lexical::Token(TokenType::kSemiColon, -1, -1));
        }

        return body_statement;
    } else {
        auto body_statement =
            ParserFactory::ParseAST<AST::ASTNode>(tokens, context);

        if (!body_statement) {
            DLOG(ERROR) << "Parse body stmt failed";
            return nullptr;
        }

        if (!body_statement->HasBody()) {
            // consume ';'
            MYCC_CheckAndConsume_ReturnNull(TokenType::kSemiColon, tokens)
        }

        return body_statement;
    }
}

std::tuple<Lexical::Token, std::list<std::unique_ptr<AST::ASTNode>>>
ParseVariable(               // NOLINT
    SyntaxContext& context,  // NOLINT
    TokenList& tokens) {
    auto variable_name = tokens.pop();
    std::list<std::unique_ptr<AST::ASTNode>> array_shape;

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
                    ParserFactory::ParseAST<AST::ASTNode>(tokens, context);
                if (array_size == nullptr)
                    return std::make_tuple(
                        Lexical::Token(TokenType::kUnknown, -1, -1),
                        std::move(array_shape));

                // consume "]"
                if (!checkAndPrint(TokenType::kRBracket, tokens)) {
                    return std::make_tuple(
                        Lexical::Token(TokenType::kUnknown, -1, -1),
                        std::move(array_shape));
                } else {
                    tokens.pop();  // consume "]"
                    array_shape.push_back(std::move(array_size));
                }
            } else {
                MYCC_PrintFirstTokenError(tokens,
                                          "have incomplete type") return std::
                    make_tuple(Lexical::Token(TokenType::kUnknown, -1, -1),
                               std::move(array_shape));
            }
        }

        // return final variable and its shape
        return std::make_tuple(variable_name, std::move(array_shape));
    } else {
        return std::make_tuple(variable_name, std::move(array_shape));
    }
}

std::tuple<std::shared_ptr<AST::Type>, Lexical::Token> ParseTypeDecl(
    const std::string& type_name, TokenList& tokens, SyntaxContext& context) {
    /**
     * Parse name of variable
     */
    if (tokens.peek().Type() == TokenType::kIdentity) {
        auto [variable_name, array_shape] = ParseVariable(context, tokens);

        // check if variable name is valid
        if (variable_name.Type() == TokenType::kUnknown) {
            return std::make_tuple(
                nullptr, Lexical::Token(TokenType::kUnknown, -1, -1));
        }

        // check if current type is array
        if (!array_shape.empty()) {
            auto attrs = tokens.LoadCachedAttributes();
            auto context_type = std::make_shared<AST::Type>(type_name);
            return std::make_tuple(
                context.getArrayType(context_type, attrs, array_shape),
                variable_name);
        } else {
            std::list<TokenType> attrs_token;
            auto attrs = tokens.LoadCachedAttributes();

            for (auto& attr : attrs) {
                attrs_token.push_back(attr.Type());
            }

            return std::make_tuple(
                std::make_shared<AST::Type>(type_name, attrs_token),
                variable_name);
        }
    }

    /**
     * Parse Function Pointer RetType
     */
    else if (tokens.peek().Type() == TokenType::kLParentheses) {
        tokens.pop();

        /**
         * Parser Function Pointer name
         */
        // consume "*"
        if (!checkAndPrint(TokenType::kReference, tokens)) {
            return std::make_tuple(
                nullptr, Lexical::Token(TokenType::kUnknown, -1, -1));
        }

        // get function Pointer name
        if (!checkAndPrint(TokenType::kIdentity, tokens)) {
            return std::make_tuple(
                nullptr, Lexical::Token(TokenType::kUnknown, -1, -1));
        }
        auto name = tokens.pop();

        // consume ")"
        if (!checkAndPrint(TokenType::kRParentheses, tokens)) {
            return std::make_tuple(
                nullptr, Lexical::Token(TokenType::kUnknown, -1, -1));
        }

        /**
         * Parser Function Pointer ArgumentList
         */
        // consume "("
        if (!checkAndPrint(TokenType::kLParentheses, tokens)) {
            return std::make_tuple(
                nullptr, Lexical::Token(TokenType::kUnknown, -1, -1));
        }

        // consume function Pointer argument list
        std::list<std::shared_ptr<AST::Type>> func_arguments;
        while (tokens.peek().Type() != TokenType::kRParentheses) {
            // token list should not be empty
            if (tokens.size() == 1) {
                MYCC_PrintFirstTokenError(
                    tokens, "Except an ) but got nothing") return std::
                    make_tuple(nullptr,
                               Lexical::Token(TokenType::kUnknown, -1, -1));
            }

            // get base type
            auto types = ParseTypeName(tokens, context);
            auto func_args_type =
                ParseTypeDecl(TokenListToString(types), tokens, context);
            if (std::get<0>(func_args_type) == nullptr)
                return std::make_tuple(
                    nullptr, Lexical::Token(TokenType::kUnknown, -1, -1));

            func_arguments.push_back(std::get<0>(func_args_type));

            // consume ","
            if (tokens.peek().Type() != TokenType::kRParentheses) {
                if (!checkAndPrint(TokenType::kComma, tokens)) {
                    return std::make_tuple(
                        nullptr, Lexical::Token(TokenType::kUnknown, -1, -1));
                }
            }
        }

        // consume ")"
        if (!checkAndPrint(TokenType::kRParentheses, tokens)) {
            return std::make_tuple(nullptr,
                                   Lexical::Token(TokenType::kUnknown, -1, -1));
        }

        return std::make_tuple(context.getFuncPtrType(type_name), name);
    }

    /**
     * We are allowed no identifier after type
     */
    else {
        if (context.hasType(type_name)) {
            std::list<TokenType> attrs_token;
            auto attrs = tokens.LoadCachedAttributes();
            for (auto& attr : attrs) {
                attrs_token.push_back(attr.Type());
            }
            return std::make_tuple(
                std::make_shared<AST::Type>(type_name, attrs_token),
                Lexical::Token(TokenType::kUnknown, -1, -1));
        } else {
            // TODO we need print more useful error message
            DLOG(WARNING) << "RetType " << type_name << " is not defined";
            return std::make_tuple(nullptr,
                                   Lexical::Token(TokenType::kUnknown, -1, -1));
        }
    }
}
}  // namespace Hzcc::Syntax::Parser
