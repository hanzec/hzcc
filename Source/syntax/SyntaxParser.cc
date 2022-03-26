#include <algorithm>
#include <array>
#include <list>
#include <memory>

#include "AST/ast_context.h"
#include "lexical/Token.h"
#include "lexical/utils/token_utils.h"
#include "syntax/Parser.h"
#include "syntax/syntax.h"
#include "utils/Status.h"
#include "utils/message_utils.h"
#include "utils/token_list_utils.h"

namespace Mycc::Syntax {

using namespace TokenListUtils;
using TokenList = std::list<Lexical::Token>;

static TokenList GetAttribute(TokenList& tokens) {
    TokenList attribute;
    while (!tokens.empty() &&
           Lexical::TokenUtils::IsAttribute(peek(tokens).Type())) {
        attribute.push_back(tokens.front());
        tokens.pop_front();
    }
    return attribute;
}

Status GenerateAST(AST::ASTContext& context, TokenList& tokens) {
    // in the most top level of the file only contains function definitions,
    // function prototypes and variables declarations
    while (!tokens.empty()) {
        // get attributes
        auto attrs = GetAttribute(tokens);

        // handle enumerable definition
        if (tokens.front().Type() == Lexical::kEnum ||
            tokens.front().Type() == Lexical::kUnion ||
            tokens.front().Type() == Lexical::kStruct ||
            tokens.front().Type() == Lexical::kTypedef) {
            // parsing as statement declaration
            context.addASTNode(
                ParserFactory::ParseAST<AST::ASTNode>(context, tokens, attrs));
        }
        // handle declaration
        else if (context.hasType(peek(tokens).Value())) {
            auto type = context.getType(pop(tokens).Value());

            // there is some possibility that some attributes are after the type
            {
                auto attr_cont = GetAttribute(tokens);
                attrs.insert(attr_cont.begin(), attr_cont.begin(),
                             attr_cont.end());
            }

            // check next token should be an identifier
            if (peek(tokens).Type() == Lexical::kIdentity) {
                // if we found "(" then it is a function declaration
                if (peek2(tokens).Type() == Lexical::kLParentheses) {
                    if (!context.addASTNode(
                            ParserFactory::ParseAST<AST::FunctionNode>(
                                context, tokens, attrs))) {
                        DLOG(ERROR) << "Failed to parse function declaration";
                        return {Status::Code::SyntaxError,
                                "Failed to parse function declaration"};
                    }
                } else {
                    if (!context.addASTNode(
                            ParserFactory::ParseAST<AST::ASTNode>(
                                context, tokens, attrs))) {
                        DLOG(ERROR) << "Failed to parse variable declaration";
                        return {Status::Code::SyntaxError,
                                "Failed to parse variable declaration"};
                    }

                    // statement will end with semicolon
                    if (peek(tokens).Type() == Lexical::kSemiColon) {
                        pop(tokens);
                    } else {
                        MYCC_PrintFirstTokenError(
                            tokens,
                            "Expect an ; after the type definition, but got " +
                                error_token.Value());
                        return {Status::INVALID_ARGUMENT, "unexpected token"};
                    }
                }
            } else {
                MYCC_PrintFirstTokenError(
                    tokens,
                    "Expect an identifier after the type definition, but got " +
                        error_token.Value());
                return {Status::INVALID_ARGUMENT, "unexpected token"};
            }
        }
        // handle error
        else {
            MYCC_PrintFirstTokenError(
                tokens, "Unexpected token: " + error_token.Value());
            return {Status::INVALID_ARGUMENT, "unexpected token"};
        }
    }
    return Status::OkStatus();
}

}  // namespace Mycc::Syntax