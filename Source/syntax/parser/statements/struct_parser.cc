//
// Created by chen_ on 2022/3/27.
//

#include "struct_parser.h"

#include <cassert>

#include "AST/CompilationUnit.h"
#include "AST/stmt/StructDeclStmt.h"
#include "syntax/utils/common_utils.h"
#include "utils/type_name_utils.h"

namespace Hzcc::Syntax::Parser {

StructDeclare::StructDeclare() noexcept
    : ParserBase(TypeNameUtil::hash<AST::StructDeclStmt>(),
                 TypeNameUtil::name_pretty<AST::StructDeclStmt>()) {}

std::shared_ptr<AST::StructType> StructDeclare::parse_internal(
    TokenList& tokens, SyntaxContext& context) {
    // consume struc
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kStruct, tokens);

    // consume struct_name
    if (tokens.peek().Type() != Lexical::TokenType::kIdentity) {
        MYCC_PrintFirstTokenError_ReturnNull(tokens,
                                             "struct struct_name expected");
    }

    auto struct_name = tokens.pop().Value();

    // consume {
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLBrace, tokens);

    // parse struct body
    std::list<Lexical::Token> current_attr_token =
        tokens.LoadCachedAttributes();
    auto struct_node = context.addStructType(struct_name, current_attr_token);
    while (tokens.peek().Type() != Lexical::TokenType::kRBrace) {
        if (tokens.peek().Type() == Lexical::TokenType::kStruct &&
            tokens.peek3().Type() == Lexical::TokenType::kLBrace) {
            if (!struct_node->AddChild(parse_internal(tokens, context))) {
                DLOG(ERROR)
                    << "parse sub-struct in [" << struct_name << "] failed";
                return nullptr;
            }
        }

        // consume type
        else {
            auto start = tokens.peek();
            auto type_name = ParseTypeName(tokens, context);
            auto [type, var_name] =
                ParseTypeDecl(TokenListToString(type_name), tokens, context);
            auto inner_struct_attributes = tokens.LoadCachedAttributes();

            if (type == nullptr) {
                MYCC_PrintTokenError_ReturnNull(start, "Expected type name");
            }

            if (!struct_node->AddChild(var_name.Value(), type,
                                       inner_struct_attributes)) {
                DLOG(ERROR) << "parse struct field in [" << var_name.Value()
                            << "] failed";
                return nullptr;
            }

            // if define multiple value, we need to consume ','
            if (tokens.peek().Type() == Lexical::TokenType::kComma) {
                while (tokens.peek().Type() == Lexical::TokenType::kComma) {
                    tokens.pop();

                    // next token should be identifier (not one of type)
                    if (tokens.peek().Type() != Lexical::TokenType::kIdentity ||
                        context.hasType(tokens.peek().Value())) {
                        MYCC_PrintFirstTokenError_ReturnNull(
                            tokens, "need an identifier here");
                    }

                    // parse type
                    auto [variable, array_shape] =
                        ParseVariable(context, tokens);

                    // check if type is valid
                    if (variable.Location().first == -1) {
                        DLOG(ERROR) << "Parse array variable failed";
                        return nullptr;
                    }

                    // add variable to context
                    if (array_shape.empty()) {
                        if (!struct_node->AddChild(variable.Value(), type,
                                                   inner_struct_attributes)) {
                            return nullptr;
                        }
                    } else {
                        auto array_type = context.getArrayType(
                            type, inner_struct_attributes, array_shape);
                        if (!struct_node->AddChild(variable.Value(), array_type,
                                                   inner_struct_attributes)) {
                            return nullptr;
                        }
                    }
                }
            }

            // consume ;
            MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kSemiColon,
                                            tokens);
        }
    }

    // consume }
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRBrace, tokens);

    return std::move(struct_node);
}

std::unique_ptr<AST::ASTNode> StructDeclare::parse_impl(
    TokenList& tokens, SyntaxContext& context) {
    // parsing struct
    auto ref = tokens.peek();
    std::shared_ptr<AST::StructType> struct_node =
        parse_internal(tokens, context);
    if (!struct_node) {
        DLOG(ERROR) << "parse struct failed";
        return nullptr;
    }

    return std::make_unique<AST::StructDeclStmt>(struct_node->GetName(),
                                                 struct_node, ref.Location());
}
}  // namespace Hzcc::Syntax::Parser
