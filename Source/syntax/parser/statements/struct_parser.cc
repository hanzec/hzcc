//
// Created by chen_ on 2022/3/27.
//

#include "struct_parser.h"

#include <cassert>

#include "AST/ASTContext.h"
#include "AST/statement/struct.h"
#include "syntax/utils/common_utils.h"
#include "syntax/utils/token_list_utils.h"
#include "utils/type_name_utils.h"
namespace Mycc::Syntax::Parser {
using namespace TokenListUtils;

StructDeclare::StructDeclare() noexcept
    : ParserBase(TypeNameUtil::hash<AST::StructDeclareNode>(),
                 TypeNameUtil::name_pretty<AST::StructDeclareNode>()) {}

std::shared_ptr<AST::StructType> StructDeclare::parse_internal(
    AST::ASTContext& context, TokenList& tokens, TokenList& attributes) {
    // consume struct
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kStruct, tokens);

    // consume struct_name
    if (peek(tokens).Type() != Lexical::TokenType::kIdentity) {
        MYCC_PrintFirstTokenError_ReturnNull(tokens,
                                             "struct struct_name expected");
    }

    auto struct_name = pop_list(tokens).Value();

    // consume {
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLBrace, tokens);

    // collecting attributes
    std::list<Lexical::TokenType> struct_attributes;
    for (const auto& attribute : attributes) {
        struct_attributes.push_back(attribute.Type());
        attributes.pop_front();
    }

    // parse struct body
    auto struct_node = context.addStructType(struct_name, struct_attributes);
    while (TokenListUtils::peek(tokens).Type() != Lexical::TokenType::kRBrace) {
        // we also allowed struct inside struct
        ConcatAttribute(attributes, tokens);

        if (TokenListUtils::peek(tokens).Type() ==
                Lexical::TokenType::kStruct &&
            TokenListUtils::peek3(tokens).Type() ==
                Lexical::TokenType::kLBrace) {
            if (!struct_node->AddChild(
                    parse_internal(context, tokens, attributes))) {
                DLOG(ERROR)
                    << "parse sub-struct in [" << struct_name << "] failed";
                return nullptr;
            }
        }

        // consume type
        else {
            // get attrs
            std::list<Lexical::TokenType> inner_struct_attributes;
            for (const auto& attribute : attributes) {
                inner_struct_attributes.push_back(attribute.Type());
                attributes.pop_front();
            }

            auto start = tokens.front();
            auto type_name = ParseTypeName(context, tokens);
            auto [type, attrs, var_name] =
                ParseTypeDecl(TokenListToString(type_name), context, tokens);

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
            if (tokens.front().Type() == Lexical::TokenType::kComma) {
                while (tokens.front().Type() == Lexical::TokenType::kComma) {
                    pop_list(tokens);

                    // next token should be identifier (not one of type)
                    if (tokens.front().Type() !=
                            Lexical::TokenType::kIdentity ||
                        context.hasType(tokens.front().Value())) {
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
                        auto array_type = context.getType(type, array_shape);
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
    AST::ASTContext& context, TokenList& tokens, TokenList& attributes) {
    ConcatAttribute(attributes, tokens);

    // parsing struct
    auto ref = tokens.front();
    std::shared_ptr<AST::StructType> struct_node =
        parse_internal(context, tokens, attributes);
    if (!struct_node) {
        DLOG(ERROR) << "parse struct failed";
        return nullptr;
    }

    return std::make_unique<AST::StructDeclareNode>(struct_node->GetName(), ref,
                                                    struct_node);
}
}  // namespace Mycc::Syntax::Parser
