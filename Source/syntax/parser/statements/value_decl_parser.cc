//
// Created by chen_ on 2022/3/26.
//

#include "value_decl_parser.h"

#include "AST/ast_context.h"
#include "AST/statement/value_decl.h"
#include "lexical/Token.h"
#include "syntax/utils/common_utils.h"
#include "utils/type_name_utils.h"
namespace Mycc::Syntax::Parser {
ValueDeclare::ValueDeclare() noexcept
    : Statement(TypeNameUtil::hash<AST::VarDecl>(),
                TypeNameUtil::name_pretty<AST::VarDecl>()) {}

std::unique_ptr<AST::ASTNode> ValueDeclare::parse_impl(
    Mycc::AST::ASTContext& context, Mycc::Syntax::TokenList& tokens,
    Mycc::Syntax::TokenList& attributes) {
    // consume type and val define
    auto type_name = ParseTypeName(context, tokens);
    auto [type, attrs, name] =
        ParseTypeDecl(TokenListToString(type_name), context, tokens);

    // check if type is valid
    if (!type) {
        DLOG(ERROR) << "Parse type failed";
        return nullptr;
    }

    // create final attrs type
    auto final_attr = TokenList(attributes.begin(), attributes.end());
    final_attr.insert(final_attr.end(), attrs.begin(), attrs.end());

    // for a function definition, we only allow one per line.
    if (type->IsFuncPtr()) {
        if (tokens.front().Type() != Lexical::TokenType::kSemiColon) {
            DLOG(ERROR) << "Expect \";\" after function pointer";
            MYCC_PrintFirstTokenError_ReturnNull(
                tokens, "Expect \";\" after function pointer");
        }
    }

    // check if current statement is a function definition
    if (peek(tokens).Type() == Lexical::TokenType::kLParentheses) {
        MYCC_PrintFirstTokenError_ReturnNull(
            tokens, "Function definition not allowed here");
    }

    // add new variable
    std::unique_ptr<AST::VarDecl> var_decl{nullptr};
    if (tokens.front().Type() == Lexical::TokenType::kAssign) {
        // consume assign
        pop_list(tokens);

        // check duplicate variable
        if (context.hasVariable(name.Value(), true)) {
            MYCC_PrintFirstTokenError_ReturnNull(tokens,
                                                 "Duplicate variable name");
        }

        // parse initial value
        auto value_node = tokens.front();
        auto value = ParseConditionalExpr(context, tokens);

        // check if value is valid
        if (!value) {
            DLOG(ERROR) << "Parse initial value for " << name.Value()
                        << "failed";
            return nullptr;
        }

        // check type compatibility
        if (Options::Global_enable_type_checking) {
            Message::set_current_part("Type checking");

            // type shoule be compatible
            if (!type->IsSame(value->GetType())) {
                auto orig_type = value->GetType();
                value = AST::ASTNode::CastTo(type, std::move(value));
                if (value == nullptr) {
                    MYCC_PrintTokenError_ReturnNull(
                        value_node,
                        "Type mismatch, "
                        "expected " +
                            type->GetName() + ", got " + orig_type->GetName());
                }
            }
        }

        Message::set_current_part("Parser");
    }

    // if define multiple value, we will add type back to tokens and parse in
    // next parser call
    if (tokens.front().Type() == Lexical::TokenType::kComma) {
        pop_list(tokens);  // consume ',' since we don't need it
        auto error_attrs = GetAttribute(tokens);
        if (!error_attrs.empty()) {
            MYCC_PrintFirstTokenError_ReturnNull(
                error_attrs, error_token.Value() + " is not allowed here");
        }

        // push all types back to tokens
        tokens.insert(tokens.begin(), type_name.begin(), type_name.end());
        tokens.push_front(
            Lexical::Token(Lexical::TokenType::kSemiColon, -1, -1));
    }

    // create new variable if not in root context
    if (!context.AtRot()) {
        auto status = context.addVariable(name.Value(), type);
        if (!status.Ok()) {
            MYCC_PrintTokenError(name, status.Message());
            return nullptr;
        }
    }

    return std::make_unique<AST::VarDecl>(type, final_attr, name);
}

}  // namespace Mycc::Syntax::Parser