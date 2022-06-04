//
// Created by chen_ on 2022/3/26.
//

#include "value_decl_parser.h"

#include "AST/CompilationUnit.h"
#include "AST/stmt/VarDecl.h"
#include "lexical/Token.h"
#include "syntax/SyntaxContext.h"
#include "syntax/utils/common_utils.h"
#include "utils/type_name_utils.h"
namespace Hzcc::Syntax::Parser {
ValueDeclare::ValueDeclare() noexcept
    : Statement(TypeNameUtil::hash<AST::VarDecl>(),
                TypeNameUtil::name_pretty<AST::VarDecl>()) {}

std::unique_ptr<AST::ASTNode> ValueDeclare::parse_impl(
    TokenList& tokens, Hzcc::Syntax::SyntaxContext& context) {
    // consume type and val define
    auto attrs = tokens.LoadCachedAttributes();
    auto type_name = ParseTypeName(tokens, context);
    auto [type, name] =
        ParseTypeDecl(TokenListToString(type_name), tokens, context);

    // check if type is valid
    if (!type) {
        DLOG(ERROR) << "Parse type failed";
        return nullptr;
    }

    // if not fist declare, cannot have any attributes
    if (!_is_fist_declare) {
        if (attrs.size() != attribute_size_) {
            MYCC_PrintFirstTokenError_ReturnNull(
                tokens, "Expected ';' at end of declaration");
        }
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
                " line " + std::to_string(line_no));
    }

    // variable cannot be defined as void
    if (type->GetName(true) == "void") {
        MYCC_PrintFirstTokenError_ReturnNull(
            tokens, "Variable '" + name.Value() + "' has type void");
    }

    // for a function definition, we only allow one per line.
    if (type->IsFuncPtr()) {
        if (tokens.peek().Type() != Lexical::TokenType::kSemiColon) {
            DLOG(ERROR) << "Expect \";\" after function pointer";
            MYCC_PrintFirstTokenError_ReturnNull(
                tokens, "Expect \";\" after function pointer");
        }
    }

    // check if current stmt is a function definition
    if (tokens.peek().Type() == Lexical::TokenType::kLParentheses) {
        MYCC_PrintFirstTokenError_ReturnNull(
            tokens, "Function definition not allowed here");
    }

    // add new variable
    std::unique_ptr<AST::ASTNode> var_value{nullptr};
    if (tokens.peek().Type() == Lexical::TokenType::kAssign) {
        // consume assign
        tokens.pop();

        // parse initial value
        auto value_node = tokens.peek();
        auto value = ParseConditionalExpr(context, tokens);

        // check if value is valid
        if (!value) {
            DLOG(ERROR) << "Parse initial value for " << name.Value()
                        << "failed";
            return nullptr;
        }

        // check type compatibility

        // type should be compatible
        if (*type == *value->RetType()) {
            auto orig_type = value->RetType();
            value = AST::ASTNode::CastTo(type, std::move(value));
            if (value == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    value_node,
                    "Initialization for ' " + name.Value() +
                        "' has wrong type: " + orig_type->GetName() +
                        " expected: " + type->GetName());
            }
        }
        var_value = std::move(value);
    }

    // if define multiple value, we will add type back to tokens and parse in
    // next parser call
    if (tokens.peek().Type() == Lexical::TokenType::kComma) {
        tokens.pop();  // consume ',' since we don't need it
        _is_fist_declare = false;

        // push all types back to tokens
        attribute_size_ = attrs.size();
        tokens.insert_front(attrs.begin(), attrs.end());
        tokens.insert_front(type_name.begin(), type_name.end());
        tokens.push(Lexical::Token(Lexical::TokenType::kSemiColon, -1, -1));
    } else {
        attribute_size_ = 0;
        _is_fist_declare = true;
    }

    // create new variable if not in root context
    if (!context.AtRoot()) {
        if (context.hasVariable(name.Value(), true)) {
            MYCC_PrintFirstTokenError_ReturnNull(tokens,
                                                 "Duplicate variable name");
        }
        context.addVariable(name.Location().first, name.Value(), type);
    }

    return std::make_unique<AST::VarDecl>(name.Location(), type, name.Value(),
                                          std::move(var_value));
}

}  // namespace Hzcc::Syntax::Parser
