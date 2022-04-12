#include <algorithm>
#include <array>
#include <list>
#include <memory>

#include "AST/ASTContext.h"
#include "lexical/Token.h"
#include "lexical/utils/token_utils.h"
#include "syntax/Parser.h"
#include "syntax/syntax.h"
#include "syntax/utils/common_utils.h"
#include "utils/Status.h"
#include "utils/message_utils.h"
#include "utils/token_list_utils.h"
namespace Mycc::Syntax {
using namespace TokenListUtils;

Status GenerateAST(AST::ASTContext& context, TokenList& tokens) {
    // in the most top level of the file only contains function definitions,
    // function prototypes and variables declarations
    while (!tokens.empty()) {
        auto attrs = GetAttribute(tokens);

        // handle enumerable definition
        if (tokens.front().Type() == Lexical::kEnum ||
            tokens.front().Type() == Lexical::kUnion) {
            // parsing as statement declaration
            context.addDecl(
                ParserFactory::ParseAST<AST::ASTNode>(context, tokens, attrs));
        }
        // ignore single semicolon
        else if (tokens.front().Type() == Lexical::kSemiColon) {
            pop_list(tokens);
            DVLOG(SYNTAX_LOG_LEVEL) << "ignore single semicolon";
        } else {
            // get type name
            auto type_name = Parser::ParseTypeName(context, tokens);

            // get attributes
            ConcatAttribute(attrs, tokens);

            // if struct or union, parse as struct or union
            if (peek(tokens).Type() == Lexical::kLBrace &&
                type_name.front().Type() == Lexical::kStruct) {
                // parsing as function definition
                tokens.insert(tokens.begin(), type_name.begin(),
                              type_name.end());
                auto new_node = ParserFactory::ParseAST<AST::StructDeclareNode>(
                    context, tokens, attrs);

                if (new_node != nullptr) {
                    context.addDecl(std::move(new_node));
                } else {
                    DVLOG(SYNTAX_LOG_LEVEL)
                        << "failed to parse struct declaration";
                    return {Status::Code::SyntaxError,
                            "Failed to parse struct declaration"};
                }

            }

            // if funcdecl
            else if (tokens.size() >= 2 &&
                     peek2(tokens).Type() == Lexical::kLParentheses) {
                tokens.insert(tokens.begin(), type_name.begin(),
                              type_name.end());

                auto new_node = ParserFactory::ParseAST<AST::FunctionDeclNode>(
                    context, tokens, attrs);

                if (new_node != nullptr) {
                    context.addDecl(std::move(new_node));
                } else {
                    DVLOG(SYNTAX_LOG_LEVEL)
                        << "Failed to parse function declaration";
                    return {Status::Code::SyntaxError,
                            "Failed to parse function declaration"};
                }
            } else if (tokens.size() >= 2 &&
                       (peek2(tokens).Type() == Lexical::kSemiColon ||
                        peek2(tokens).Type() == Lexical::kComma ||
                        peek2(tokens).Type() == Lexical::kAssign ||
                        peek2(tokens).Type() == Lexical::kLBracket)) {
                tokens.insert(tokens.begin(), type_name.begin(),
                              type_name.end());

                auto new_node = ParserFactory::ParseAST<AST::ASTNode>(
                    context, tokens, attrs);
                if (new_node != nullptr) {
                    context.addDecl(std::move(new_node));
                } else {
                    DVLOG(SYNTAX_LOG_LEVEL)
                        << "Failed to parse variable declaration";
                    return {Status::Code::SyntaxError,
                            "Failed to parse variable declaration"};
                }

                // statement will end with semicolon
                if (peek(tokens).Type() == Lexical::kSemiColon) {
                    pop_list(tokens);
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
                if (tokens.front().Type() == Lexical::TokenType::kLBrace &&
                    !context.AtRoot()) {
                    MYCC_PrintFirstTokenError(tokens, "Unmatched '{'");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                } else if (tokens.front().Type() == Lexical::TokenType::kType) {
                    MYCC_PrintFirstTokenError(tokens, "Expected identifier");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                } else if (tokens.front().Type() ==
                               Lexical::TokenType::kSemiColon &&
                           !type_name.empty()) {
                    MYCC_PrintFirstTokenError(tokens, "Expected identifier");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                } else if (tokens.front().Type() ==
                           Lexical::TokenType::kComma) {
                    MYCC_PrintFirstTokenError(tokens, "Expected identifier");
                    return {Status::INVALID_ARGUMENT, "unexpected token"};
                } else if (tokens.front().Type() ==
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

}  // namespace Mycc::Syntax
