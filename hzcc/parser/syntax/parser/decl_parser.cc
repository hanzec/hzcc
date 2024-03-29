//
// Created by chen_ on 2022/12/27.
//
#include <glog/logging.h>

#include <list>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#include "ast/CompilationUnit.h"
#include "ast/Stmt.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "enums.h"
#include "options.h"
#include "parser/common/Token.h"
#include "parser/common/token_type.h"
#include "parser/parser.h"
#include "parser/syntax/common_utils.h"
#include "parser_factory.h"
#include "syntax_parser.h"
#include "utils/logging.h"
#include "utils/status/status.h"
#include "utils/status/statusor.h"
#include "utils/type_name_utils.h"

namespace hzcc::syntax::parser {
DeclStatement::DeclStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::IDeclStmt>(),
                 TypeNameUtil::name_pretty<ast::IDeclStmt>()) {}

StatusOr<VarPack> DeclStatement::parse_type(TokenList& tokens, SyntaxCtx& context,
                                            ast::QualTypePtr& base_type) {
    // for *
    while (tokens.peek().type() == TokenType::Mul) {
        if (base_type->is<TypeCategory::Ptr>()) {
            tokens.pop();
            base_type = std::make_shared<ast::QualType>(
                std::make_shared<ast::PointerType>(base_type));
        } else {
            return syntax::utils::TokenErr(tokens, "Cannot use '*' on non-pointer type");
        }
    }

    // get decl to_str
    if (!tokens.empty() && tokens.peek().type() != TokenType::kIdentity) {
        return syntax::utils::TokenErr(tokens.peek(), "except a identifier");
    }
    auto decl_name = tokens.pop();

    // for []
    if (tokens.peek().type() == TokenType::LBracket) {
        tokens.pop();
        if (tokens.peek().type() != TokenType::RBracket) {
            HZCC_CHECK_OR_ASSIGN(size_expr, Parser::Parse<ast::Expr>(context, tokens))

            // check match ]
            if (tokens.peek().type() != TokenType::RBracket) {
                return syntax::utils::TokenErr(tokens.peek(), "except a ']'");
            }

            // global scope does not allow vla
            if (context->at_root() && !size_expr->type()->is<Qualifier::CONST>()) {
                return syntax::utils::TokenErr(
                    tokens.peek(),
                    "Variable length array declaration not allowed at file "
                    "scope");
            }

            base_type = std::make_shared<ast::QualType>(
                std::make_shared<ast::ArrayType>(std::move(size_expr), base_type));
        }
        tokens.pop();
    }

    return std::make_pair(base_type, decl_name);
}

StatusOr<ast::StmtPtr> DeclStatement::parse_impl(SyntaxCtx context, TokenList& tokens) {
    // handle the case enum/struct/union since they don't include * or & on
    // their declaration
    if (tokens.peek3().type() == TokenType::LBrace) {
        if (tokens.peek().type() == TokenType::Enum ||
            tokens.peek().type() == TokenType::Union) {
            return syntax::utils::TokenErr(
                tokens, absl::StrCat(magic_enum::enum_name(tokens.peek().type()),
                                     " definition are not supported yet!"));
        } else if (tokens.peek().type() == TokenType::Struct) {
            return parse_struct(context, tokens);
        }
    }

    // get base type
    HZCC_CHECK_OR_ASSIGN(base_type,  // NOLINT
                         utils::get_base_type(tokens, context))

    ast::QualTypePtr final_type(base_type);
    while (tokens.peek().type() == TokenType::Mul) {
        if (base_type->is<TypeCategory::Array>()) {
            tokens.pop();
            final_type = std::make_shared<ast::QualType>(
                std::make_shared<ast::PointerType>(final_type));
        } else {
            return syntax::utils::TokenErr(tokens, "Cannot use '*' on non-pointer type");
        }
    }

    // parse rest of the declaration
    if (tokens.peek2().type() == TokenType::LParentheses) {
        // function declaration
        return parse_func(tokens, context, final_type);
    } else {
        // variable declaration
        HZCC_CHECK_OR_ASSIGN(new_node, Parser::Parse<ast::VarDecl>(context, tokens))
        return new_node;
    }
}

StatusOr<ast::StmtPtr> DeclStatement::parse_var(TokenList& tokens, SyntaxCtx& context,
                                                ast::QualTypePtr base_type,
                                                ast::QualTypePtr curr_type) {
    if (tokens.peek().type() != TokenType::kIdentity) {
        return syntax::utils::TokenErr(tokens.peek(), "except a identifier");
    }

    auto name = tokens.pop();

    // check if current variable has initializer
    std::list<std::unique_ptr<ast::VarDecl>> var_list;
    if (tokens.peek().type() == TokenType::Assign) {
        tokens.pop();
        // parse initializer
        HZCC_CHECK_OR_ASSIGN(init_expr, Parser::Parse<ast::Expr>(context, tokens))

        var_list.emplace_back(std::make_unique<ast::VarDecl>(
            name.loc(), name.to_str(), std::move(init_expr),
            std::make_unique<ast::TypeProxyExpr>(init_expr->loc(), curr_type)));
    } else {
        var_list.emplace_back(std::make_unique<ast::VarDecl>(
            name.loc(), name.to_str(), std::make_unique<ast::EmptyExpr>(),
            std::make_unique<ast::TypeProxyExpr>(name.loc(), curr_type)));
    }

    // add all new variable to the current scope
    while (tokens.peek().type() == TokenType::Comma) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(var_pack,  // NOLINT
                             parse_type(tokens, context, base_type))

        // try parse initializer
        if (tokens.peek().type() == TokenType::Assign) {
            tokens.pop();
            // parse initializer
            HZCC_CHECK_OR_ASSIGN(init_expr, Parser::Parse<ast::Expr>(context, tokens))

            var_list.emplace_back(std::make_unique<ast::VarDecl>(
                name.loc(), name.to_str(), std::move(init_expr),
                std::make_unique<ast::TypeProxyExpr>(name.loc(), curr_type)));
        } else {
            var_list.emplace_back(std::make_unique<ast::VarDecl>(
                name.loc(), name.to_str(), std::make_unique<ast::EmptyExpr>(),
                std::make_unique<ast::TypeProxyExpr>(name.loc(), curr_type)));
        }
    }

    return std::make_unique<ast::DeclStmt>(name.loc(), std::move(var_list));
}

StatusOr<ast::StmtPtr> DeclStatement::parse_func(TokenList& tokens, SyntaxCtx& context,
                                                 ast::QualTypePtr return_type) {
    // get function to_str
    auto func_name = tokens.pop();

    // function can only be defined in global scope
    if (!context->at_root()) {
        return syntax::utils::TokenErr(func_name,
                                       "function can only be defined in global scope");
    }

    // function name cannot duplicate with any other variable to_str
    if (!Options::Global_allow_same_name_for_func_val &&
        context->has_var(func_name.to_str(), true)) {
        return CompileError(func_name.loc(),
                            absl::StrCat("Function '", func_name.to_str(),
                                         "' already defined as global variable"));
    }

    // function can not define twice with different return type
    if (context->has_func(func_name.to_str()) &&
        context->func_ret_type(func_name.to_str()) != return_type) {
        return CompileError(
            func_name.loc(),
            absl::StrCat("Function '", func_name.to_str(),
                         "' already defined with different "
                         "return type (Previous: ",
                         context->func_ret_type(func_name.to_str())->to_str(),
                         ", Current: ", return_type->to_str(), ")"));
    }

    /**
     * Parse Argument List
     */
    // consume '('
    HZCC_CheckAndConsume_ReturnErr(TokenType::LParentheses, tokens)
        // next token should not be ',', if it is, it means empty argument list
        if (tokens.peek().type() == TokenType::Comma) {
        return syntax::utils::TokenErr(tokens, "Expected type to_str");
    }

    // actual function node
    auto func_node = std::make_unique<ast::FuncDeclStmt>(
        func_name.loc(), func_name.to_str(),
        std::make_unique<ast::TypeProxyExpr>(func_name.loc(), return_type));

    // parse argument list
    bool first = false;
    std::list<VarPack> arg_type_list;
    do {
        // consume ',' if available
        if (tokens.peek().type() == TokenType::Comma) {
            tokens.pop();
        } else {
            // directly break if ')'
            if (tokens.peek().type() == TokenType::RParentheses) {
                break;
            } else if (first) {
                return syntax::utils::TokenErr(tokens, "Expected ','");
            }
        }

        // get base argument_type
        HZCC_CHECK_OR_ASSIGN(base_type,  // NOLINT
                             utils::get_base_type(tokens, context))

        HZCC_CHECK_OR_ASSIGN(var_pack,                                // NOLINT
                             parse_type(tokens, context, base_type))  // NOLINT

        first = true;
        arg_type_list.emplace_back(var_pack.first, var_pack.second);
        func_node->AddFunctionArgument(std::make_unique<ast::ParamVarDecl>(
            var_pack.second.loc(), var_pack.second.to_str(),
            std::make_unique<ast::TypeProxyExpr>(var_pack.second.loc(), var_pack.first)));
    } while (!tokens.empty() && tokens.peek().type() != TokenType::RParentheses);

    // check argument list is matched with previous definition
    if (context->has_func(func_name.to_str())) {
        int index = 0;
        auto func_param = context->func_param_types(func_name.to_str());
        for (auto& [arg_type, token] : arg_type_list) {
            if (arg_type != func_param[index++]) {
                return CompileError(
                    token.loc(),
                    absl::StrCat("Function '", func_name.to_str(),
                                 "' already defined with different "
                                 "argument type at index ",
                                 index, " (Previous: ",
                                 context->func_ret_type(func_name.to_str())->to_str(),
                                 ", Current: ", return_type->to_str(), ")"));
            }
        }
    }

    // check ')'  and consume
    HZCC_CheckAndConsume_ReturnErr(TokenType::RParentheses, tokens);

    // trying to parse the function body if it exists
    if (tokens.peek().type() == TokenType::LBrace) {
        // we do not allow multiple implementation of the same function
        if (context->has_func(func_name.to_str()) &&
            context->has_body(func_name.to_str())) {
            return CompileError(
                func_name.loc(),
                absl::StrCat("Function '", func_name.to_str(), "' already defined"));
        }

        // create a new function scope
        context->create_func(func_name.to_str(), return_type);

        // trying to syntax the function body
        HZCC_CHECK_OR_ASSIGN(func_body, Parser::Parse<ast::CompoundStmt>(context, tokens))

        // if function body is syntax correct, then add it to the function
        // node and back to current the scope
        func_node->set_body(std::move(func_body));
        context->leave_scope();
    }

    // if the function body is not exits, then we need to check the ";"
    else if (tokens.peek().type() == TokenType::SemiColon) {
        tokens.pop();
    }

    // if the function body is not exits, then we need to check the ";"
    else {
        return syntax::utils::TokenErr(tokens,
                                       "Except an \";\" after function declaration");
    }
    return std::move(func_node);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::RecordDEclPtr> DeclStatement::parse_struct(SyntaxCtx& context,
                                                         TokenList& tokens) {
    // consume struct
    HZCC_CheckAndConsume_ReturnErr(TokenType::Struct, tokens);

    // consume struct_name
    if (tokens.peek().type() != TokenType::kIdentity) {
        return syntax::utils::TokenErr(tokens, "struct struct_name expected");
    }

    auto struct_name = tokens.pop();

    // consume {
    HZCC_CheckAndConsume_ReturnErr(TokenType::LBrace, tokens);

    // add struct to context first to avoid recursive struct definition
    // convert all token type to Attr
    auto base_type_list = tokens.cache_attr_list();
    std::vector<Qualifier> ret_attr_list;
    for (auto& attr : base_type_list) {
        ret_attr_list.emplace_back(to_attr(attr.type()));
    }

    // for vardecl and funcdecl, we need to consume the *
    auto struct_type = context->add_struct_type(struct_name.to_str(), ret_attr_list);

    // construct struct node
    auto struct_node = std::make_unique<ast::RecordDecl>(
        struct_name.loc(), struct_name.to_str(),
        std::make_unique<ast::TypeProxyExpr>(struct_name.loc(), struct_type));

    // parse struct body
    while (tokens.peek().type() != TokenType::RBrace) {
        if (tokens.peek().type() == TokenType::Struct &&
            tokens.peek3().type() == TokenType::LBrace) {
            // parse nested struct
            HZCC_CHECK_OR_ASSIGN(inner_struct,                   // NOLINT
                                 parse_struct(context, tokens))  // NOLINT;
            struct_node->add_field(std::move(inner_struct));
        }

        // consume type
        else {
            auto start = tokens.peek();
            HZCC_CHECK_OR_ASSIGN(                       // NOLINT
                base_type,                              // NOLINT
                utils::get_base_type(tokens, context))  // NOLINT

            // parse variable declaration
            HZCC_CHECK_OR_ASSIGN(                        // NOLINT
                var_decl,                                // NOLINT
                parse_type(tokens, context, base_type))  // NOLINT

            // add variable to struct node
            struct_node->add_field(std::make_unique<ast::FieldDecl>(  // NOLINT
                var_decl.second.loc(), var_decl.second.to_str(),
                std::make_unique<ast::TypeProxyExpr>(var_decl.second.loc(),
                                                     var_decl.first)));

            // struct cannot have initializer
            if (tokens.peek().type() == TokenType::Assign) {
                return syntax::utils::TokenErr(tokens, "Struct cannot have initializer");
            }

            // if define multiple value, we need to consume ','
            while (tokens.peek().type() == TokenType::Comma) {
                tokens.pop();

                HZCC_CHECK_OR_ASSIGN(                       // NOLINT
                    base_type_inner,                        // NOLINT
                    utils::get_base_type(tokens, context))  // NOLINT

                // parse variable declaration
                HZCC_CHECK_OR_ASSIGN(                              // NOLINT
                    var_decl_inner,                                // NOLINT
                    parse_type(tokens, context, base_type_inner))  // NOLINT

                // add variable to struct type and struct node
                struct_node->add_field(std::make_unique<ast::FieldDecl>(  // NOLINT
                    var_decl_inner.second.loc(), var_decl_inner.second.to_str(),
                    std::make_unique<ast::TypeProxyExpr>(var_decl_inner.second.loc(),
                                                         var_decl_inner.first)));

                // struct cannot have initializer
                if (tokens.peek().type() == TokenType::Assign) {
                    return syntax::utils::TokenErr(tokens,
                                                   "Struct cannot have initializer");
                }
            }

            // consume ;
            HZCC_CheckAndConsume_ReturnErr(TokenType::SemiColon, tokens)
        }
    }

    // consume }
    HZCC_CheckAndConsume_ReturnErr(TokenType::RBrace, tokens);

    return std::move(struct_node);
}
#pragma clang diagnostic pop
}  // namespace hzcc::syntax::parser