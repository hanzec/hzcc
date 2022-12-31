//
// Created by chen_ on 2022/12/27.
//
#include <list>

#include "ast/CompilationUnit.h"
#include "ast/cast/Cast.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "parser/syntax/common_utils.h"
#include "parser_factory.h"
#include "syntax_parser.h"
namespace hzcc::syntax::parser {
DeclStatement::DeclStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::IDeclStmt>(),
                 TypeNameUtil::name_pretty<ast::IDeclStmt>()) {}

template <typename T_LIST = std::list<Token>>
static StatusOr<T_LIST> check_duplicate(const T_LIST& tokens) {
    T_LIST ret;
    for (auto& token : tokens) {
        if (std::find(ret.begin(), ret.end(), token) != ret.end()) {
            return hzcc::syntax::utils::TokenErr(
                token, "duplicate identifier " + token.val());
        } else {
            ret.push_back(token);
        }
    }
    return ret;
}

StatusOr<VarPack> DeclStatement::parse_type(TokenList& tokens,
                                            SyntaxCtx& context,
                                            ast::TypePtr& base_type) {
    // for *
    while (tokens.peek().Type() == TokenType::kMul) {
        if (base_type->is_ptr()) {
            tokens.pop();
            base_type = std::make_shared<ast::PointerType>(base_type);
        } else {
            return syntax::utils::TokenErr(
                tokens, "Cannot use '*' on non-pointer type");
        }
    }

    // get decl name
    if (!tokens.empty() && tokens.peek().Type() != TokenType::kIdentity) {
        return syntax::utils::TokenErr(tokens.peek(), "except a identifier");
    }
    auto decl_name = tokens.pop();

    // for []
    if (tokens.peek().Type() == TokenType::kLBracket) {
        tokens.pop();
        if (tokens.peek().Type() != TokenType::kRBracket) {
            HZCC_CHECK_OR_ASSIGN(size_expr,
                                 Parser::Parse<ast::Expr>(context, tokens))

            // check match ]
            if (tokens.peek().Type() != TokenType::kRBracket) {
                return syntax::utils::TokenErr(tokens.peek(), "except a ']'");
            }

            // global scope does not allow vla
            if (context->at_root() &&
                !size_expr->retType()->is<Attribute::kCONST>()) {
                return syntax::utils::TokenErr(
                    tokens.peek(),
                    "Variable length array declaration not allowed at file "
                    "scope");
            }

            base_type = std::make_shared<ast::ArrayType>(base_type,
                                                         std::move(size_expr));
        }
        tokens.pop();
    }

    return std::make_pair(base_type, decl_name);
}

StatusOr<ast::StmtPtr> DeclStatement::parse_impl(SyntaxCtx context,
                                                 TokenList& tokens) {
    // handle the case enum/struct/union since they don't include * or & on
    // their declaration
    if (tokens.peek3().Type() == TokenType::kLBrace) {
        if (tokens.peek().Type() == TokenType::kEnum ||
            tokens.peek().Type() == TokenType::kUnion) {
            return syntax::utils::TokenErr(
                tokens, hzcc::to_string(tokens.peek().Type()) +
                            " definition are not supported yet!");
        } else if (tokens.peek().Type() == TokenType::kStruct) {
            return parse_struct(context, tokens);
        }
    }

    // get base type
    HZCC_CHECK_OR_ASSIGN(raw_base_type,  // NOLINT
                         utils::get_base_type(tokens, context))

    // check base type is valid
    auto base_type_str = hzcc::to_string(raw_base_type);
    if (!context->has_type(base_type_str)) {
        return syntax::utils::TokenErr(raw_base_type.second,
                                       "Unknown type: " + base_type_str);
    }

    // convert all token type to Attr
    auto base_type_list = tokens.cache_attr_list();
    std::list<Attribute> ret_attr_list;
    for (auto& attr : base_type_list) {
        ret_attr_list.emplace_back(to_attr(attr.Type()));
    }

    // for vardecl and funcdecl, we need to consume the *
    std::shared_ptr<ast::Type> base_type =
        context->get_type(base_type_str)->type_of(ret_attr_list);
    std::shared_ptr<ast::Type> final_type(base_type);
    while (tokens.peek().Type() == TokenType::kMul) {
        if (base_type->is_ptr()) {
            tokens.pop();
            final_type = std::make_shared<ast::PointerType>(final_type);
        } else {
            return syntax::utils::TokenErr(
                tokens, "Cannot use '*' on non-pointer type");
        }
    }

    // parse rest of the declaration
    if (tokens.peek2().Type() == TokenType::kLParentheses) {
        // function declaration
        return parse_func(tokens, context, final_type);
    } else {
        // variable declaration
        HZCC_CHECK_OR_ASSIGN(new_node,
                             Parser::Parse<ast::VarDecl>(context, tokens))
        return new_node;
    }
}

StatusOr<ast::StmtPtr> DeclStatement::parse_var(TokenList& tokens,
                                                SyntaxCtx& context,
                                                ast::TypePtr base_type,
                                                ast::TypePtr curr_type) {
    if (tokens.peek().Type() != TokenType::kIdentity) {
        return syntax::utils::TokenErr(tokens.peek(), "except a identifier");
    }

    auto name = tokens.pop();

    // check if current variable has initializer
    std::list<std::unique_ptr<ast::VarDecl>> var_list;
    if (tokens.peek().Type() == TokenType::kAssign) {
        tokens.pop();
        // parse initializer
        HZCC_CHECK_OR_ASSIGN(init_expr,
                             Parser::Parse<ast::Expr>(context, tokens))

        var_list.emplace_back(std::make_unique<ast::VarDecl>(
            name.loc(), name.val(), std::move(init_expr),
            std::make_unique<ast::TypeProxyExpr>(init_expr->loc(), curr_type)));
    } else {
        var_list.emplace_back(std::make_unique<ast::VarDecl>(
            name.loc(), name.val(), std::make_unique<ast::EmptyExpr>(),
            std::make_unique<ast::TypeProxyExpr>(name.loc(), curr_type)));
    }

    // add all new variable to the current scope
    while (tokens.peek().Type() == TokenType::kComma) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(var_pack,  // NOLINT
                             parse_type(tokens, context, base_type))

        // try parse initializer
        if (tokens.peek().Type() == TokenType::kAssign) {
            tokens.pop();
            // parse initializer
            HZCC_CHECK_OR_ASSIGN(init_expr,
                                 Parser::Parse<ast::Expr>(context, tokens))

            var_list.emplace_back(std::make_unique<ast::VarDecl>(
                name.loc(), name.val(), std::move(init_expr),
                std::make_unique<ast::TypeProxyExpr>(name.loc(), curr_type)));
        } else {
            var_list.emplace_back(std::make_unique<ast::VarDecl>(
                name.loc(), name.val(), std::make_unique<ast::EmptyExpr>(),
                std::make_unique<ast::TypeProxyExpr>(name.loc(), curr_type)));
        }
    }

    return std::make_unique<ast::DeclStmt>(name.loc(), std::move(var_list));
}

StatusOr<ast::StmtPtr> DeclStatement::parse_func(TokenList& tokens,
                                                 SyntaxCtx& context,
                                                 ast::TypePtr return_type) {
    // get function name
    auto func_name = tokens.pop();

    // function can only be defined in global scope
    if (!context->at_root()) {
        return syntax::utils::TokenErr(
            func_name, "function can only be defined in global scope");
    }

    /**
     * Parse Argument List
     */
    // consume '('
    HZCC_CheckAndConsume_ReturnErr(TokenType::kLParentheses, tokens)

        // next token should not be ',', if it is, it means empty argument list
        if (tokens.peek().Type() == TokenType::kComma) {
        return syntax::utils::TokenErr(tokens, "Expected type name");
    }

    // actual function node
    auto func_node = std::make_unique<ast::FuncDeclStmt>(
        func_name.loc(), func_name.val(),
        std::make_unique<ast::TypeProxyExpr>(func_name.loc(), return_type));

    // parse argument list
    bool first = false;
    std::list<ast::TypePtr> arg_type_list;
    do {
        // consume ',' if available
        if (tokens.peek().Type() == TokenType::kComma) {
            tokens.pop();
        } else {
            // directly break if ')'
            if (tokens.peek().Type() == TokenType::kRParentheses) {
                break;
            } else if (first) {
                return syntax::utils::TokenErr(tokens, "Expected ','");
            }
        }

        // get base argument_type
        HZCC_CHECK_OR_ASSIGN(raw_base_type,  // NOLINT
                             utils::get_base_type(tokens, context))

        // check base type is valid
        auto base_type_str = hzcc::to_string(raw_base_type);
        if (!context->has_type(base_type_str)) {
            return syntax::utils::TokenErr(raw_base_type.second,
                                           "Unknown type: " + base_type_str);
        }

        // convert all token type to Attr
        auto base_type_list = tokens.cache_attr_list();
        std::list<Attribute> ret_attr_list;
        for (auto& attr : base_type_list) {
            ret_attr_list.emplace_back(to_attr(attr.Type()));
        }

        // for vardecl and funcdecl, we need to consume the *
        std::shared_ptr<ast::Type> base_type =
            context->get_type(base_type_str)->type_of(ret_attr_list);

        HZCC_CHECK_OR_ASSIGN(var_pack,                                // NOLINT
                             parse_type(tokens, context, base_type))  // NOLINT

        first = true;
        arg_type_list.emplace_back(var_pack.first);
        func_node->AddFunctionArgument(std::make_unique<ast::ParamVarDecl>(
            var_pack.second.loc(), var_pack.second.val(),
            std::make_unique<ast::TypeProxyExpr>(var_pack.second.loc(),
                                                 var_pack.first)));
    } while (!tokens.empty() &&
             tokens.peek().Type() != TokenType::kRParentheses);

    // check ')'  and consume
    HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens);

    // trying to parse the function body if it exists
    if (tokens.peek().Type() == TokenType::kLBrace) {
        // create a new named scope
        context->new_scope(func_name.val(), return_type);

        // trying to syntax the function body
        HZCC_CHECK_OR_ASSIGN(func_body,
                             Parser::Parse<ast::CompoundStmt>(context, tokens))

        // if function body is syntax correct, then add it to the function
        // node and back to current the scope
        func_node->set_body(std::move(func_body));
        context->leave_scope();
    }

    // if the function body is not exits, then we need to check the ";"
    else if (tokens.peek().Type() == TokenType::kSemiColon) {
        tokens.pop();
    }

    // if the function body is not exits, then we need to check the ";"
    else {
        return syntax::utils::TokenErr(
            tokens, "Except an \";\" after function declaration");
    }
    return std::move(func_node);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::RecordDEclPtr> DeclStatement::parse_struct(SyntaxCtx& context,
                                                         TokenList& tokens) {
    // consume struct
    HZCC_CheckAndConsume_ReturnErr(TokenType::kStruct, tokens);

    // consume struct_name
    if (tokens.peek().Type() != TokenType::kIdentity) {
        return syntax::utils::TokenErr(tokens, "struct struct_name expected");
    }

    auto struct_name = tokens.pop();

    // consume {
    HZCC_CheckAndConsume_ReturnErr(TokenType::kLBrace, tokens);

    // add struct to context first to avoid recursive struct definition
    // convert all token type to Attr
    auto base_type_list = tokens.cache_attr_list();
    std::list<Attribute> ret_attr_list;
    for (auto& attr : base_type_list) {
        ret_attr_list.emplace_back(to_attr(attr.Type()));
    }

    // for vardecl and funcdecl, we need to consume the *
    auto struct_type =
        context->add_struct_type(struct_name.val(), ret_attr_list);

    // construct struct node
    auto struct_node = std::make_unique<ast::RecordDecl>(
        struct_name.loc(), struct_name.val(),
        std::make_unique<ast::TypeProxyExpr>(struct_name.loc(), struct_type));

    // parse struct body
    while (tokens.peek().Type() != TokenType::kRBrace) {
        if (tokens.peek().Type() == TokenType::kStruct &&
            tokens.peek3().Type() == TokenType::kLBrace) {
            // parse nested struct
            HZCC_CHECK_OR_ASSIGN(inner_struct,                   // NOLINT
                                 parse_struct(context, tokens))  // NOLINT;
            struct_node->add_field(std::move(inner_struct));
        }

        // consume type
        else {
            auto start = tokens.peek();
            HZCC_CHECK_OR_ASSIGN(                       // NOLINT
                raw_base_type,                          // NOLINT
                utils::get_base_type(tokens, context))  // NOLINT

            // check base type is valid
            auto base_type_str = hzcc::to_string(raw_base_type);
            if (!context->has_type(base_type_str)) {
                return syntax::utils::TokenErr(
                    raw_base_type.second, "Unknown type: " + base_type_str);
            }

            // add struct to context first to avoid recursive struct definition
            // convert all token type to Attr
            ret_attr_list.clear();
            base_type_list = tokens.cache_attr_list();
            for (auto& attr : base_type_list) {
                ret_attr_list.emplace_back(to_attr(attr.Type()));
            }

            // for vardecl and funcdecl, we need to consume the *
            std::shared_ptr<ast::Type> base_type =
                context->get_type(base_type_str)->type_of(ret_attr_list);

            // parse variable declaration
            HZCC_CHECK_OR_ASSIGN(                        // NOLINT
                var_decl,                                // NOLINT
                parse_type(tokens, context, base_type))  // NOLINT

            // add variable to struct node
            struct_node->add_field(std::make_unique<ast::FieldDecl>(  // NOLINT
                var_decl.second.loc(), var_decl.second.val(),
                std::make_unique<ast::TypeProxyExpr>(var_decl.second.loc(),
                                                     var_decl.first)));

            // struct cannot have initializer
            if (tokens.peek().Type() == TokenType::kAssign) {
                return syntax::utils::TokenErr(
                    tokens, "Struct cannot have initializer");
            }

            // if define multiple value, we need to consume ','
            while (tokens.peek().Type() == TokenType::kComma) {
                tokens.pop();

                HZCC_CHECK_OR_ASSIGN(                       // NOLINT
                    raw_base_type_inner,                    // NOLINT
                    utils::get_base_type(tokens, context))  // NOLINT

                // check base type is valid
                auto base_type_str_inner = hzcc::to_string(raw_base_type_inner);
                if (!context->has_type(base_type_str_inner)) {
                    return syntax::utils::TokenErr(
                        raw_base_type_inner.second,
                        "Unknown type: " + base_type_str_inner);
                }

                // add struct to context first to avoid recursive struct
                // definition convert all token type to Attr
                ret_attr_list.clear();
                base_type_list = tokens.cache_attr_list();
                for (auto& attr : base_type_list) {
                    ret_attr_list.emplace_back(to_attr(attr.Type()));
                }

                // get actual base type
                auto base_type_inner =
                    context->get_struct_type(base_type_str_inner)
                        ->type_of(ret_attr_list);

                // parse variable declaration
                HZCC_CHECK_OR_ASSIGN(                              // NOLINT
                    var_decl_inner,                                // NOLINT
                    parse_type(tokens, context, base_type_inner))  // NOLINT

                // add variable to struct type and struct node
                struct_type->AddChild(var_decl_inner.second.val(),
                                      var_decl_inner.first);
                struct_node->add_field(
                    std::make_unique<ast::FieldDecl>(  // NOLINT
                        var_decl_inner.second.loc(),
                        var_decl_inner.second.val(),
                        std::make_unique<ast::TypeProxyExpr>(
                            var_decl_inner.second.loc(),
                            var_decl_inner.first)));

                // struct cannot have initializer
                if (tokens.peek().Type() == TokenType::kAssign) {
                    return syntax::utils::TokenErr(
                        tokens, "Struct cannot have initializer");
                }
            }

            // consume ;
            HZCC_CheckAndConsume_ReturnErr(TokenType::kSemiColon, tokens)
        }
    }

    // consume }
    HZCC_CheckAndConsume_ReturnErr(TokenType::kRBrace, tokens);

    return std::move(struct_node);
}
#pragma clang diagnostic pop
}  // namespace hzcc::syntax::parser