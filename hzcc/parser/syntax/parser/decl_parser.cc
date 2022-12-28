//
// Created by chen_ on 2022/12/27.
//
#include <list>

#include "ast/CompilationUnit.h"
#include "ast/cast/CastTool.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "parser/syntax/parser_factory.h"
#include "parser/syntax/utils/common_utils.h"
#include "syntax_parser.h"

namespace hzcc::syntax::parser {
DeclStatement::DeclStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::IDeclStmt>(),
                 TypeNameUtil::name_pretty<ast::IDeclStmt>()) {}

ALWAYS_INLINE StatusOr<TypeNamePair> get_base_type(TokenList& tokens,
                                                   hzcc::syntax::Ctx& context) {
    auto fist_token = tokens.pop();
    if (fist_token.Type() == TokenType::kEnum ||
        fist_token.Type() == TokenType::kUnion ||
        fist_token.Type() == TokenType::kStruct) {
        // next token should be an identifier
        if (tokens.peek().Type() != TokenType::kIdentity) {
            return hzcc::syntax::utils::TokenErr(tokens.peek(),
                                                 "except a identifier");
        } else {
            if (context.hasType("struct " + tokens.peek().Value())) {
                return hzcc::syntax::utils::TokenErr(
                    tokens.peek(),
                    "struct " + tokens.peek().Value() + " has been defined");
            } else {
                return std::make_pair(fist_token.Type(), tokens.pop());
            }
        }
    } else {
        if (context.hasType(fist_token.Value())) {
            return std::make_pair(fist_token.Type(), fist_token);
        } else {
            return hzcc::syntax::utils::TokenErr(
                fist_token,
                "type " + fist_token.Value() + " has not been defined");
        }
    }
}

template <typename T_LIST = std::list<Token>>
static StatusOr<T_LIST> check_duplicate(const T_LIST& tokens) {
    T_LIST ret;
    for (auto& token : tokens) {
        if (std::find(ret.begin(), ret.end(), token) != ret.end()) {
            return hzcc::syntax::utils::TokenErr(
                token, "duplicate identifier " + token.Value());
        } else {
            ret.push_back(token);
        }
    }
    return ret;
}

template <typename VarPack = std::pair<ast::TypePtr, Token>>
static StatusOr<VarPack> parse_type(TokenList& tokens,             // NOLINT
                                    ast::TypePtr& base_type,       // NOLINT
                                    hzcc::syntax::Ctx& context) {  // NOLINT
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

    // test if decl name is already defined
    if (context.hasVariable(decl_name.Value(), true) ||
        (!Options::Global_allow_same_name_for_func_val &&
         context.hasVariable(decl_name.Value(), true))) {
        return syntax::utils::TokenErr(
            decl_name, "var " + decl_name.Value() + " has been defined");
    }

    // for []
    if (tokens.peek().Type() == TokenType::kLBracket) {
        tokens.pop();
        if (tokens.peek().Type() != TokenType::kRBracket) {
            HZCC_CHECK_OR_ASSIGN(size_expr,
                                 Parser::Parse<ast::Expr>(tokens, context))

            // check match ]
            if (tokens.peek().Type() != TokenType::kRBracket) {
                return syntax::utils::TokenErr(tokens.peek(), "except a ']'");
            }

            // global scope does not allow vla
            if (context.isRoot() &&
                !size_expr->retType()->is<ast::Attribute::kCONST>()) {
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

StatusOr<ast::StmtPtr> DeclStatement::parse_impl(TokenList& tokens,
                                                 hzcc::syntax::Ctx& context) {
    // handle the case enum/struct/union since they don't include * or & on
    // their declaration
    if (tokens.peek3().Type() == TokenType::kLBrace) {
        if (tokens.peek().Type() == TokenType::kEnum ||
            tokens.peek().Type() == TokenType::kUnion) {
            return syntax::utils::TokenErr(
                tokens, hzcc::to_string(tokens.peek().Type()) +
                            " definition are not supported yet!");
        } else if (tokens.peek().Type() == TokenType::kStruct) {
            return parse_struct(tokens, context);
        }
    }

    // get base type
    HZCC_CHECK_OR_ASSIGN(raw_base_type,  // NOLINT
                         get_base_type(tokens, context))

    // check base type is valid
    auto base_type_str = hzcc::to_string(raw_base_type);
    if (!context.hasType(base_type_str)) {
        return syntax::utils::TokenErr(raw_base_type.second,
                                       "Unknown type: " + base_type_str);
    }

    // for vardecl and funcdecl, we need to consume the *
    std::shared_ptr<ast::Type> base_type =
        context.get_type(base_type_str, tokens.cache_attr_list());
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
        return parse_func(tokens, final_type, context);
    } else {
        // variable declaration
        HZCC_CHECK_OR_ASSIGN(new_node,
                             Parser::Parse<ast::VarDecl>(tokens, context))
        return new_node;
    }
}

StatusOr<ast::StmtPtr> DeclStatement::parse_var(TokenList& tokens,
                                                ast::TypePtr base_type,
                                                ast::TypePtr curr_type,
                                                hzcc::syntax::Ctx& context) {
    if (tokens.peek().Type() != TokenType::kIdentity) {
        return syntax::utils::TokenErr(tokens.peek(), "except a identifier");
    }

    auto decl_name = tokens.pop();

    // check duplicate variable, here we only check if the variable is
    // already defined in the current scope. since we can mask the
    // definition of the variable in the parent scope.
    if (context.hasVariable(decl_name.Value(), true)) {
        auto [is_global, line_no] = context.getVariableInfo(decl_name.Value());
        return syntax::utils::TokenErr(
            tokens,
            (context.isRoot() ? "Global variable '" : "Local variable '") +
                decl_name.Value() +
                (is_global ? "' duplicates global variable declared in\n"
                           : "' duplicates local variable declared in\n") +
                " line " + std::to_string(line_no));
    }

    // variable cannot be defined as void
    if (curr_type->is<ast::TypeCategory::kNumerical>() &&
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
        static_cast<ast::NumericalType*>(curr_type.get())
            ->is<ast::PrimitiveType::kVoid>()) {
        return syntax::utils::TokenErr(
            tokens, "Variable '" + decl_name.Value() + "' has type void");
    }

    // for a function definition, we only allow one per line.
    if (curr_type->is<ast::TypeCategory::kFuncPtr>()) {
        if (tokens.peek().Type() != TokenType::kSemiColon) {
            DLOG(ERROR) << "Expect \";\" after function pointer";
            return syntax::utils::TokenErr(
                tokens, "Expect \";\" after function pointer");
        }
    }

    // add current variable to the context
    context.addVariable(decl_name.Location(), decl_name.Value(), curr_type);

    // check if current variable has initializer
    std::list<std::unique_ptr<ast::VarDecl>> var_list;
    if (tokens.peek().Type() == TokenType::kAssign) {
        tokens.pop();
        // parse initializer
        HZCC_CHECK_OR_ASSIGN(init_expr,
                             Parser::Parse<ast::Expr>(tokens, context))

        // check if type is match
        auto init_type = init_expr->retType();
        if (init_expr->retType() != curr_type) {
            // here we're trying to cast the type to the correct type, since
            // initializer always at right side, we always need rval for this
            // cast
            auto cast_ret =
                ast::CastTool::apply(true, std::move(init_expr), curr_type);

            if (!cast_ret.ok()) {
                return syntax::utils::TokenErr(
                    tokens, "Cannot cast from " + init_type->Name() + " to " +
                                curr_type->Name() + "since :" +
                                std::string(cast_ret.status().message()));
            }

            init_expr = std::move(cast_ret.value());
        }

        var_list.emplace_back(std::make_unique<ast::VarDecl>(
            decl_name.Location(), decl_name.Value(), curr_type,
            std::move(init_expr)));
    } else {
        var_list.emplace_back(std::make_unique<ast::VarDecl>(
            decl_name.Location(), decl_name.Value(), curr_type,
            std::make_unique<ast::EmptyExpr>()));
    }

    // add all new variable to the current scope
    while (tokens.peek().Type() == TokenType::kComma) {
        tokens.pop();
        HZCC_CHECK_OR_ASSIGN(var_pack,  // NOLINT
                             parse_type(tokens, base_type, context))

        // check duplicate variable, here we only check if the variable is
        // already defined in the current scope. since we can mask the
        // definition of the variable in the parent scope.
        if (context.hasVariable(var_pack.second.Value(), true)) {
            auto [is_global, line_no] =
                context.getVariableInfo(var_pack.second.Value());
            return syntax::utils::TokenErr(
                tokens,
                (context.isRoot() ? "Global variable '" : "Local variable '") +
                    var_pack.second.Value() +
                    (is_global ? "' duplicates global variable declared in\n"
                               : "' duplicates local variable declared in\n") +
                    " line " + std::to_string(line_no));
        }

        // try parse initializer
        if (tokens.peek().Type() == TokenType::kAssign) {
            tokens.pop();
            // parse initializer
            HZCC_CHECK_OR_ASSIGN(init_expr,
                                 Parser::Parse<ast::Expr>(tokens, context))

            // check if type is match
            auto init_type = init_expr->retType();
            if (init_expr->retType() != curr_type) {
                // here we're trying to cast the type to the correct type, since
                // initializer always at right side, we always need rval for
                // this cast
                auto cast_ret = ast::CastTool::apply(
                    true, std::move(init_expr), curr_type);

                if (!cast_ret.ok()) {
                    return syntax::utils::TokenErr(
                        tokens, "Cannot cast from " + init_type->Name() +
                                    " to " + curr_type->Name() + "since :" +
                                    std::string(cast_ret.status().message()));
                }

                init_expr = std::move(cast_ret.value());
            }

            var_list.emplace_back(std::make_unique<ast::VarDecl>(
                decl_name.Location(), decl_name.Value(), curr_type,
                std::move(init_expr)));
        } else {
            var_list.emplace_back(std::make_unique<ast::VarDecl>(
                decl_name.Location(), decl_name.Value(), curr_type,
                std::make_unique<ast::EmptyExpr>()));
        }
    }

    return std::make_unique<ast::DeclStmt>(decl_name.Location(),
                                           std::move(var_list));
}

StatusOr<ast::StmtPtr> DeclStatement::parse_func(TokenList& tokens,
                                                 ast::TypePtr return_type,
                                                 hzcc::syntax::Ctx& context) {
    // get function name
    auto func_name = tokens.pop();

    // function can only be defined in global scope
    if (!context.isRoot()) {
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

    // function name cannot duplicate with any other variable name
    if (!Options::Global_allow_same_name_for_func_val &&
        context.hasVariable(func_name.Value(), true)) {
        return syntax::utils::TokenErr(
            func_name, "Function '" + func_name.Value() +
                           "' already defined as global variable");
    }

    // actual function node
    auto func_node = std::make_unique<ast::FuncDeclStmt>(
        func_name.Location(), func_name.Value(), return_type);

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
                             get_base_type(tokens, context))

        // check base type is valid
        auto base_type_str = hzcc::to_string(raw_base_type);
        if (!context.hasType(base_type_str)) {
            return syntax::utils::TokenErr(raw_base_type.second,
                                           "Unknown type: " + base_type_str);
        }

        // for vardecl and funcdecl, we need to consume the *
        std::shared_ptr<ast::Type> base_type =
            context.get_type(base_type_str, tokens.cache_attr_list());

        HZCC_CHECK_OR_ASSIGN(var_pack,                                // NOLINT
                             parse_type(tokens, base_type, context))  // NOLINT

        first = true;
        arg_type_list.emplace_back(var_pack.first);
        func_node->AddFunctionArgument(std::make_unique<ast::ParamVarDecl>(
            var_pack.second.Location(), var_pack.second.Value(),
            var_pack.first));
    } while (!tokens.empty() &&
             tokens.peek().Type() != TokenType::kRParentheses);

    // check ')'  and consume
    HZCC_CheckAndConsume_ReturnErr(TokenType::kRParentheses, tokens)

        // function with same name must match previous definition
        if (context.hasFunction(func_name.Value())) {
        auto [func_type, func_type_attributes, line_no] =
            context.getFuncRetAndArgType(func_name.Value());

        // compare argument number
        if (func_type_attributes.size() != arg_type_list.size()) {
            return syntax::utils::TokenErr(
                func_name, "Function '" + func_name.Value() +
                               "' already defined with different argument "
                               "number from previous definition in line" +
                               std::to_string(line_no.first));
        }

        // compare return type
        if (!(*func_type == *return_type)) {
            return syntax::utils::TokenErr(
                func_name, "Function " + return_type->Name() + " " +
                               func_name.Value() + " already defined with " +
                               func_type->Name() + " return type in line " +
                               std::to_string(line_no.first));
        }

        // compare all argument type
        for (const auto& type : arg_type_list) {
            if (!(*type == *func_type_attributes.front())) {
                return syntax::utils::TokenErr(
                    func_name, "Function " + func_name.Value() +
                                   " already defined with different argument "
                                   "type in line " +
                                   std::to_string(line_no.first));
            }
            func_type_attributes.pop_front();
        }
    }

    // add function definition to context
    context.addFunction(func_name.Location(), func_name.Value(), return_type,
                        arg_type_list);

    // trying to parse the function body if it exists
    if (tokens.peek().Type() == TokenType::kLBrace) {
        // create a new named scope
        context.enterScope(func_name.Value(), return_type);

        // add all arguments to the scope
        for (auto& arg : func_node->getArguments()) {
            context.addVariable(std::get<2>(arg), std::get<0>(arg),
                                std::get<1>(arg));
        }

        // trying to syntax the function body
        HZCC_CHECK_OR_ASSIGN(
            func_body,
            ParserFactory::ParseAST<ast::CompoundStmt>(tokens, context))

        // if function body is syntax correct, then add it to the function
        // node and back to current the scope
        func_node->set_body(std::move(func_body));
        context.leave_scope();
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

    // check function name is redefinition or not only when the function body is
    // exits
    if (func_node->HasBody() && context.hasFunction(func_name.Value()) &&
        context.hasFunctionBody(func_name.Value())) {
        return syntax::utils::TokenErr(
            func_name, "Function '" + func_name.Value() + "' already defined");
    }

    return std::move(func_node);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<DeclStatement::RecordPtr> DeclStatement::parse_struct(
    TokenList& tokens, hzcc::syntax::Ctx& context) {
    // consume struct
    HZCC_CheckAndConsume_ReturnErr(TokenType::kStruct, tokens)

        // consume struct_name
        if (tokens.peek().Type() != TokenType::kIdentity) {
        return syntax::utils::TokenErr(tokens, "struct struct_name expected");
    }

    auto struct_name = tokens.pop();

    // consume {
    HZCC_CheckAndConsume_ReturnErr(TokenType::kLBrace, tokens)

        // add struct to context first to avoid recursive struct definition
        auto current_attr_token = tokens.cache_attr_list();
    auto struct_type =
        context.addStructType(struct_name.Value(), current_attr_token);

    // construct struct node
    auto struct_node = std::make_unique<ast::RecordDecl>(struct_name.Location(),
                                                         struct_name.Value());

    // parse struct body
    while (tokens.peek().Type() != TokenType::kRBrace) {
        if (tokens.peek().Type() == TokenType::kStruct &&
            tokens.peek3().Type() == TokenType::kLBrace) {
            // parse nested struct
            HZCC_CHECK_OR_ASSIGN(inner_struct,                   // NOLINT
                                 parse_struct(tokens, context))  // NOLINT;
            struct_node->add_field(std::move(inner_struct));
        }

        // consume type
        else {
            auto start = tokens.peek();
            HZCC_CHECK_OR_ASSIGN(                // NOLINT
                raw_base_type,                   // NOLINT
                get_base_type(tokens, context))  // NOLINT

            // check base type is valid
            auto base_type_str = hzcc::to_string(raw_base_type);
            if (!context.hasType(base_type_str)) {
                return syntax::utils::TokenErr(
                    raw_base_type.second, "Unknown type: " + base_type_str);
            }

            // get actual base type
            std::shared_ptr<ast::Type> base_type =
                context.get_type(base_type_str, tokens.cache_attr_list());

            // parse variable declaration
            HZCC_CHECK_OR_ASSIGN(                        // NOLINT
                var_decl,                                // NOLINT
                parse_type(tokens, base_type, context))  // NOLINT

            // add variable to struct type and struct node
            struct_type->AddChild(var_decl.second.Value(), var_decl.first);
            struct_node->add_field(std::make_unique<ast::FieldDecl>(  // NOLINT
                var_decl.second.Location(), var_decl.second.Value(),
                var_decl.first));

            // struct cannot have initializer
            if (tokens.peek().Type() == TokenType::kAssign) {
                return syntax::utils::TokenErr(
                    tokens, "Struct cannot have initializer");
            }

            // if define multiple value, we need to consume ','
            while (tokens.peek().Type() == TokenType::kComma) {
                tokens.pop();

                HZCC_CHECK_OR_ASSIGN(                // NOLINT
                    raw_base_type_inner,             // NOLINT
                    get_base_type(tokens, context))  // NOLINT

                // check base type is valid
                auto base_type_str_inner = hzcc::to_string(raw_base_type_inner);
                if (!context.hasType(base_type_str_inner)) {
                    return syntax::utils::TokenErr(
                        raw_base_type_inner.second,
                        "Unknown type: " + base_type_str_inner);
                }

                // get actual base type
                std::shared_ptr<ast::Type> base_type_inner = context.get_type(
                    base_type_str_inner, tokens.cache_attr_list());

                // parse variable declaration
                HZCC_CHECK_OR_ASSIGN(                              // NOLINT
                    var_decl_inner,                                // NOLINT
                    parse_type(tokens, base_type_inner, context))  // NOLINT

                // add variable to struct type and struct node
                struct_type->AddChild(var_decl_inner.second.Value(),
                                      var_decl_inner.first);
                struct_node->add_field(
                    std::make_unique<ast::FieldDecl>(  // NOLINT
                        var_decl_inner.second.Location(),
                        var_decl_inner.second.Value(), var_decl_inner.first));

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