//
// Created by chen_ on 2022/3/26.
//

#include "value_decl_parser.h"

#include "AST/CompilationUnit.h"
#include "AST/statement/value_decl.h"
#include "lexical/Token.h"
#include "syntax/utils/common_utils.h"
#include "utils/type_name_utils.h"
namespace Mycc::Syntax::Parser {
ValueDeclare::ValueDeclare() noexcept
    : Statement(TypeNameUtil::hash<AST::VarDecl>(),
                TypeNameUtil::name_pretty<AST::VarDecl>()) {}

std::unique_ptr<AST::ASTNode> ValueDeclare::parse_impl(
    Mycc::AST::CompilationUnit& context, Mycc::Syntax::TokenList& tokens,
    Mycc::Syntax::TokenList& attributes) {
    // push attrs back to token stream
    tokens.insert(tokens.begin(), attributes.begin(), attributes.end());

    // consume type and val define
    auto type_name = ParseTypeName(context, tokens);
    auto [type, attrs, name] =
        ParseTypeDecl(TokenListToString(type_name), context, tokens);

    // check if type is valid
    if (!type) {
        DLOG(ERROR) << "Parse type failed";
        return nullptr;
    }

    // check duplicate variable, here we only check if the variable is
    // already defined in the current scope. since we can musk the
    // definition of the variable in the parent scope.
    if (context.hasVariable(name.Value(), true)) {
        auto [is_global, line_no] = context.getVariableInfo(name.Value());
        MYCC_PrintFirstTokenError_ReturnNull(
            tokens,
            (context.AtRoot() ? "Global variable '" : "Local variable '") +
                name.Value() +
                (is_global ? "' duplicates global variable declared in\n"
                           : "' duplicates local variable declared in\n") +
                "\t\tfile " + context.GetFileName() + " line " +
                std::to_string(line_no));
    }

    // variable cannot be defined as void
    if (type->GetName(true) == "void") {
        MYCC_PrintFirstTokenError_ReturnNull(
            tokens, "Variable '" + name.Value() + "' has type void");
    }

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
                        "Initialization for ' " + name.Value() +
                            "' has wrong type: " + orig_type->GetName() +
                            " expected: " + type->GetName());
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

        // push attrs back to tokens
        tokens.insert(tokens.begin(), attrs.begin(), attrs.end());

        tokens.push_front(
            Lexical::Token(Lexical::TokenType::kSemiColon, -1, -1));
    }

    // create new variable if not in root context
    if (!context.AtRoot()) {
        if (context.hasVariable(name.Value(), true)) {
            MYCC_PrintFirstTokenError_ReturnNull(tokens,
                                                 "Duplicate variable name");
        }
        context.addVariable(name.Location().first, name.Value(), type);
    }
    attributes.clear();

    return std::make_unique<AST::VarDecl>(type, attrs, name);
}

}  // namespace Mycc::Syntax::Parser
