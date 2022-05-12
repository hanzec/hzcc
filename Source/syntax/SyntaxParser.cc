#include <algorithm>
#include <array>
#include <list>
#include <memory>
#include <utility>

#include "AST/CompilationUnit.h"
#include "lexical/Token.h"
#include "syntax/Parser.h"
#include "syntax/syntax.h"
#include "syntax/utils/common_utils.h"
#include "utils/Status.h"
#include "utils/message_utils.h"
namespace Hzcc::Syntax {
Status GenerateAST(TokenList& tokens,
                   std::shared_ptr<AST::CompilationUnit> compile_unit) {
    // generate SyntaxContext from compile_unit
    SyntaxContext context(std::move(compile_unit));

    // in the most top level of the file only contains function definitions,
    // function prototypes and variables declarations
    while (!tokens.empty()) {
        // handle enumerable or union definition
        if (tokens.peek().Type() == Lexical::kEnum ||
            tokens.peek().Type() == Lexical::kUnion) {
            auto token_type = tokens.pop().Type();
            if (!context.hasType(tokens.peek().Value() + " " +
                                 tokens.peek2().Value())) {
                if (token_type == Lexical::kEnum) {
                    MYCC_PrintFirstTokenError(
                        tokens, "Enum definition are not supported yet!");
                    return {Status::INVALID_ARGUMENT,
                            "Enum definition are not supported yet!"};
                } else {
                    MYCC_PrintFirstTokenError(
                        tokens, "Union definition are not supported yet!");
                    return {Status::INVALID_ARGUMENT,
                            "Union definition are not supported yet!"};
                }
            } else {
                MYCC_PrintFirstTokenError(
                    tokens,
                    "Duplicate definition of type " + tokens.peek().Value());
                return {
                    Status::INVALID_ARGUMENT,
                    "Duplicate definition of type " + tokens.peek().Value()};
            }
        }
        // ignore single semicolon
        else if (tokens.peek().Type() == Lexical::kSemiColon) {
            tokens.pop();
            DVLOG(SYNTAX_LOG_LEVEL) << "ignore single semicolon";

        } else {
            // get type name
            auto type_name = Parser::ParseTypeName(tokens, context);

            // if struct or union, parse as struct or union
            if (tokens.peek().Type() == Lexical::kLBrace &&
                type_name.front().Type() == Lexical::kStruct) {
                // parsing as function definition
                tokens.insert_front(type_name.begin(), type_name.end());
                auto new_node = ParserFactory::ParseAST<AST::StructDeclareNode>(
                    tokens, context);

                if (new_node != nullptr) {
                    context.addDecl(std::move(new_node));
                } else {
                    DVLOG(SYNTAX_LOG_LEVEL)
                        << "failed to parse struct declaration";
                    return {Status::Code::SyntaxError,
                            "Failed to parse struct declaration"};
                }

            }

            // if func_decl
            else if (tokens.size() >= 2 &&
                     tokens.peek2().Type() == Lexical::kLParentheses) {
                tokens.insert_front(type_name.begin(), type_name.end());

                auto new_node = ParserFactory::ParseAST<AST::FunctionDeclNode>(
                    tokens, context);

                if (new_node != nullptr) {
                    context.addDecl(std::move(new_node));
                } else {
                    DVLOG(SYNTAX_LOG_LEVEL)
                        << "Failed to parse function declaration";
                    return {Status::Code::SyntaxError,
                            "Failed to parse function declaration"};
                }
            } else if (tokens.size() >= 2 &&
                       (tokens.peek2().Type() == Lexical::kSemiColon ||
                        tokens.peek2().Type() == Lexical::kComma ||
                        tokens.peek2().Type() == Lexical::kAssign ||
                        tokens.peek2().Type() == Lexical::kLBracket)) {
                tokens.insert_front(type_name.begin(), type_name.end());
                auto new_node =
                    ParserFactory::ParseAST<AST::VarDecl>(tokens, context);

                if (new_node != nullptr) {
                    context.addDecl(std::move(new_node));
                } else {
                    DVLOG(SYNTAX_LOG_LEVEL)
                        << "Failed to parse variable declaration";
                    return {Status::Code::SyntaxError,
                            "Failed to parse variable declaration"};
                }

                // statement will end with semicolon
                if (tokens.peek().Type() == Lexical::kSemiColon) {
                    tokens.pop();
                } else {
                    MYCC_PrintFirstTokenError(
                        tokens,
                        "Expect an ; after the type definition, but "
                        "got " +
                            error_token.Value());
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                }
            }
            // handle error
            else {
                if (tokens.peek().Type() == Lexical::TokenType::kLBrace &&
                    !context.AtRoot()) {
                    MYCC_PrintFirstTokenError(tokens, "Unmatched '{'");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                } else if (tokens.peek().Type() == Lexical::TokenType::kType) {
                    MYCC_PrintFirstTokenError(tokens, "Expected identifier");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                } else if (tokens.peek().Type() ==
                               Lexical::TokenType::kSemiColon &&
                           !type_name.empty()) {
                    MYCC_PrintFirstTokenError(tokens, "Expected identifier");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                } else if (tokens.peek().Type() == Lexical::TokenType::kComma) {
                    MYCC_PrintFirstTokenError(tokens, "Expected identifier");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                } else if (tokens.peek().Type() ==
                           Lexical::TokenType::kRBrace) {
                    MYCC_PrintFirstTokenError(tokens, "Expected identifier");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                } else if (context.AtRoot()) {
                    MYCC_PrintFirstTokenError(tokens, "Expected type name");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                } else {
                    MYCC_PrintFirstTokenError(tokens, "Expected identifier");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                }
            }
        }
    }
    return Status::OkStatus();
}

}  // namespace Hzcc::Syntax
