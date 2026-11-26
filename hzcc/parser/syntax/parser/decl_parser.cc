//
// Created by chen_ on 2022/12/27.
//
#include <absl/strings/str_cat.h>
#include <glog/logging.h>

#include <algorithm>
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
namespace {
/** @brief Determine whether a parsed bound requires a run-time value.
 * @note The constant folder only evaluates some operators. Inspect their operands so
 * constant shifts, unary expressions and sizeof remain valid file-scope bounds.
 */
bool has_runtime_bound(ast::Expr& expression) {
    if (dynamic_cast<ast::LiteralExpr*>(&expression) ||
        dynamic_cast<ast::SizeofExpr*>(&expression))
        return false;
    if (auto* op = dynamic_cast<ast::OperatorBase*>(&expression)) {
        return dynamic_cast<ast::AssignExpr*>(op) || dynamic_cast<ast::CommaExpr*>(op) ||
               has_runtime_bound(*op->lhs()) || has_runtime_bound(*op->rhs());
    }
    if (auto* unary = dynamic_cast<ast::UnaryOperator*>(&expression))
        return has_runtime_bound(*unary->expr());
    if (auto* cast = dynamic_cast<ast::CastExpr*>(&expression))
        return has_runtime_bound(*cast->cast_expr());
    if (auto* ternary = dynamic_cast<ast::TernaryExpr*>(&expression)) {
        return has_runtime_bound(*ternary->cond_expr()) ||
               has_runtime_bound(*ternary->true_expr()) ||
               has_runtime_bound(*ternary->false_expr());
    }
    return true;
}
}  // namespace

DeclStatement::DeclStatement() noexcept
    : ParserBase(TypeNameUtil::hash<ast::IDeclStmt>(),
                 TypeNameUtil::name_pretty<ast::IDeclStmt>()) {}

StatusOr<VarPack> DeclStatement::parse_type(TokenList& tokens, SyntaxCtx& context,
                                            ast::QualTypePtr base_type,
                                            bool file_scope_object) {
    if (tokens.empty())
        return utils::TokenErr(tokens, "Expected identifier before end of input");

    // for *
    while (tokens.peek().type() == TokenType::Mul) {
        auto pointer = tokens.pop();
        if (tokens.empty())
            return utils::TokenErr(pointer,
                                   "Expected identifier after '*' before end of input");
        base_type = std::make_shared<ast::QualType>(
            std::make_shared<ast::PointerType>(base_type));
    }

    // get decl to_str
    if (tokens.peek().type() != TokenType::kIdentity) {
        return utils::TokenErr(tokens, "Expected identifier");
    }
    auto name = tokens.pop();

    // for []
    std::vector<ast::ExprPtr> dimensions;
    while (!tokens.empty() && tokens.peek().type() == TokenType::LBracket) {
        const auto opening = tokens.pop();
        if (tokens.empty())
            return utils::TokenErr(opening,
                                   "Expected an array bound or ']' before end of input");
        ast::ExprPtr size = std::make_unique<ast::EmptyExpr>();
        if (tokens.peek().type() != TokenType::RBracket) {
            HZCC_CHECK_OR_ASSIGN(expr, Statement::ParseAssignExpr(tokens, context));

            // global scope does not allow vla
            if (!tokens.empty() && tokens.peek().type() == TokenType::RBracket &&
                file_scope_object && has_runtime_bound(*expr)) {
                return utils::TokenErr(
                    tokens,
                    "Variable length array declaration not allowed at file scope");
            }
            size = std::move(expr);
        }

        // check match ]
        HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::RBracket, tokens, opening));
        dimensions.push_back(std::move(size));
    }
    for (auto it = dimensions.rbegin(); it != dimensions.rend(); ++it) {
        base_type = std::make_shared<ast::QualType>(
            std::make_shared<ast::ArrayType>(std::move(*it), base_type));
    }
    return std::make_pair(base_type, name);
}

StatusOr<ast::StmtPtr> DeclStatement::parse_impl(SyntaxCtx context, TokenList& tokens) {
    if (tokens.empty())
        return utils::TokenErr(tokens, "Expected declaration before end of input");
    if (std::all_of(tokens.begin(), tokens.end(),
                    [](const Token& token) { return is_qualifier(token.type()); })) {
        return utils::TokenErr(*tokens.begin(),
                               "Expected type name after qualifier before end of input");
    }
    if (tokens.peek().type() == TokenType::RBrace) {
        return utils::TokenErr(tokens, "Expected identifier");
    }
    // handle the case enum/struct/union since they don't include * or & on
    // their declaration
    const auto start = tokens.peek();
    if ((start.type() == TokenType::Enum || start.type() == TokenType::Union) &&
        tokens.size() >= 3 && tokens.peek3().type() == TokenType::LBrace) {
        return utils::TokenErr(
            start, absl::StrCat(start.to_str(), " definitions are not supported yet"));
    }
    if (start.type() == TokenType::Struct && tokens.size() >= 3 &&
        tokens.peek3().type() == TokenType::LBrace) {
        HZCC_CHECK_OR_ASSIGN(record, parse_struct(context, tokens));
        return record;
    }

    // get base type
    HZCC_CHECK_OR_ASSIGN(base_type, utils::get_base_type(tokens, context));
    auto first_type = base_type;
    if (tokens.empty())
        return utils::TokenErr(start,
                               "Expected identifier after type before end of input");

    // for vardecl and funcdecl, we need to consume the *
    while (tokens.peek().type() == TokenType::Mul) {
        const auto pointer = tokens.pop();
        if (tokens.empty())
            return utils::TokenErr(pointer,
                                   "Expected identifier after '*' before end of input");
        first_type = std::make_shared<ast::QualType>(
            std::make_shared<ast::PointerType>(first_type));
    }
    if (tokens.peek().type() != TokenType::kIdentity) {
        return utils::TokenErr(tokens, "Expected identifier");
    }

    // parse rest of the declaration
    if (tokens.size() >= 2 && tokens.peek2().type() == TokenType::LParentheses) {
        // function declaration
        return parse_func(tokens, context, first_type);
    }

    // variable declaration
    return parse_var(tokens, context, base_type, first_type);
}

StatusOr<ast::StmtPtr> DeclStatement::parse_var(TokenList& tokens, SyntaxCtx& context,
                                                ast::QualTypePtr base_type,
                                                ast::QualTypePtr curr_type) {
    auto loc = tokens.peek().loc();
    std::list<std::unique_ptr<ast::VarDecl>> variables;
    for (;;) {
        HZCC_CHECK_OR_ASSIGN(variable,
                             parse_type(tokens, context, curr_type, context->at_root()));
        ast::ExprPtr initializer;
        if (tokens.empty())
            return utils::TokenErr(
                variable.second,
                "Expected ';' at end of declaration before end of input");

        // check if current variable has initializer
        if (tokens.peek().type() == TokenType::Assign) {
            const auto assign = tokens.pop();
            if (tokens.empty())
                return utils::TokenErr(assign,
                                       "Expected initializer before end of input");

            // parse initializer
            HZCC_CHECK_OR_ASSIGN(expr, Statement::ParseAssignExpr(tokens, context));
            initializer = std::move(expr);
        }
        const auto& name = variable.second;
        variables.push_back(std::make_unique<ast::VarDecl>(
            name.loc(), name.to_str(), std::move(initializer),
            std::make_unique<ast::TypeProxyExpr>(name.loc(), variable.first)));
        if (!tokens.cache_attr_list().empty()) {
            return utils::TokenErr(tokens, "Expected ';' at end of declaration");
        }
        if (tokens.empty())
            return utils::TokenErr(
                name, "Expected ';' at end of declaration before end of input");
        if (tokens.peek().type() != TokenType::Comma) break;
        const auto comma = tokens.pop();
        if (tokens.empty())
            return utils::TokenErr(comma,
                                   "Expected identifier after ',' before end of input");
        curr_type = base_type;
    }
    if (tokens.peek().type() != TokenType::SemiColon) {
        return utils::TokenErr(tokens, "Expected ';' at end of declaration");
    }
    tokens.pop();
    return std::make_unique<ast::DeclStmt>(loc, std::move(variables));
}

StatusOr<ast::StmtPtr> DeclStatement::parse_func(TokenList& tokens, SyntaxCtx& context,
                                                 ast::QualTypePtr return_type) {
    // get function to_str
    auto func_name = tokens.pop();

    // function can only be defined in global scope
    if (!context->at_root()) {
        return utils::TokenErr(
            tokens,
            absl::StrCat("Function definition not allowed here. Function '",
                         func_name.to_str(), "' can only be defined in global scope"));
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
        *context->func_ret_type(func_name.to_str()) != *return_type) {
        return utils::TokenErr(
            func_name, absl::StrCat("Function '", func_name.to_str(),
                                    "' already defined with different "
                                    "return type (Previous: ",
                                    context->func_ret_type(func_name.to_str())->to_str(),
                                    ", Current: ", return_type->to_str(), ")"));
    }

    /**
     * Parse Argument List
     */
    // consume '('
    const auto opening = tokens.peek();
    HZCC_CheckAndConsume_ReturnErr(TokenType::LParentheses, tokens);
    if (tokens.empty())
        return utils::TokenErr(opening,
                               "Expected parameter type or ')' before end of input");

    // next token should not be ','
    if (tokens.peek().type() == TokenType::Comma) {
        return syntax::utils::TokenErr(tokens, "Expected type name");
    }

    // actual function node
    auto func_node = std::make_unique<ast::FuncDeclStmt>(
        func_name.loc(), func_name.to_str(),
        std::make_unique<ast::TypeProxyExpr>(func_name.loc(), return_type));

    // parse argument list
    std::list<VarPack> arg_type_list;
    if (tokens.peek().type() == TokenType::Void && tokens.size() == 1) {
        return utils::TokenErr(
            tokens, "Expected ')' or parameter name after 'void' before end of input");
    }
    if (tokens.peek().type() == TokenType::Void &&
        tokens.peek2().type() == TokenType::RParentheses) {
        tokens.pop();
    } else if (tokens.peek().type() != TokenType::RParentheses) {
        for (;;) {
            // get base argument_type
            const auto parameter_start = tokens.peek();
            HZCC_CHECK_OR_ASSIGN(base_type, utils::get_base_type(tokens, context));
            if (tokens.empty())
                return utils::TokenErr(parameter_start,
                                       "Expected parameter name before end of input");
            HZCC_CHECK_OR_ASSIGN(param, parse_type(tokens, context, base_type, false));
            func_node->AddFunctionArgument(std::make_unique<ast::ParamVarDecl>(
                param.second.loc(), param.second.to_str(),
                std::make_unique<ast::TypeProxyExpr>(param.second.loc(), param.first)));
            arg_type_list.push_back(std::move(param));
            if (tokens.empty())
                return utils::Consume(TokenType::RParentheses, tokens, opening);

            // directly break if ')'
            if (tokens.peek().type() == TokenType::RParentheses) break;

            // consume ',' if available
            const auto delimiter = tokens.peek();
            HZCC_CheckAndConsume_ReturnErr(TokenType::Comma, tokens);
            if (tokens.empty())
                return utils::TokenErr(
                    delimiter, "Expected type name after ',' before end of input");
        }
    }

    // check argument list is matched with previous definition
    if (context->has_func(func_name.to_str())) {
        size_t index = 0;
        auto previous = context->func_param_types(func_name.to_str());
        if (previous.size() != arg_type_list.size()) {
            return utils::TokenErr(
                func_name,
                absl::StrCat(
                    "Function '", func_name.to_str(),
                    "' already defined with a different number of parameters (Previous: ",
                    previous.size(), ", Current: ", arg_type_list.size(), ")"));
        }
        for (const auto& [type, name] : arg_type_list) {
            if (*type != *previous[index]) {
                return utils::TokenErr(
                    name, absl::StrCat(
                              "Function '", func_name.to_str(),
                              "' already defined with different argument type at index ",
                              index + 1, " (Previous: ", previous[index]->to_str(),
                              ", Current: ", type->to_str(), ")"));
            }
            ++index;
        }
    }

    // check ')'  and consume
    HZCC_CheckAndConsume_ReturnErr(TokenType::RParentheses, tokens);

    // trying to parse the function body if it exists
    if (tokens.empty())
        return utils::TokenErr(
            func_name, "Expected ';' after function declaration before end of input");
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
        absl::Cleanup leave_scope = [&] { context->leave_scope(); };

        // trying to syntax the function body
        HZCC_CHECK_OR_ASSIGN(func_body,
                             Parser::Parse<ast::CompoundStmt>(context, tokens));

        // if function body is syntax correct, then add it to the function
        // node and back to current the scope
        func_node->set_body(std::move(func_body));
    }

    // if the function body is not exits, then we need to check the ";"
    else if (tokens.peek().type() == TokenType::SemiColon) {
        tokens.pop();
    }

    // if the function body is not exits, then we need to check the ";"
    else {
        return syntax::utils::TokenErr(tokens, "Expected ';' after function declaration");
    }
    return std::move(func_node);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
StatusOr<ast::RecordDEclPtr> DeclStatement::parse_struct(SyntaxCtx& context,
                                                         TokenList& tokens) {
    // consume struct
    const auto start = tokens.peek();
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

    const auto tag = "struct " + struct_name.to_str();
    auto struct_type =
        context->has_type(tag, true)
            ? context->get_type(tag, ret_attr_list)
            : context->add_struct_type(struct_name.to_str(), ret_attr_list);
    auto record_type = struct_type->as<ast::StructType>();
    if (!record_type->begin_definition()) {
        return utils::TokenErr(struct_name, absl::StrCat(tag, " has been defined"));
    }
    // Make the tag visible to its fields, but publish fields only on success.
    absl::Cleanup cancel_definition = [&] { record_type->end_definition(false); };
    std::list<std::unique_ptr<ast::IDeclStmt>> fields;

    // construct struct node
    auto struct_node = std::make_unique<ast::RecordDecl>(
        struct_name.loc(), struct_name.to_str(),
        std::make_unique<ast::TypeProxyExpr>(struct_name.loc(), struct_type));

    // parse struct body
    while (!tokens.empty() && tokens.peek().type() != TokenType::RBrace) {
        if (tokens.peek().type() == TokenType::Struct && tokens.size() >= 3 &&
            tokens.peek3().type() == TokenType::LBrace) {
            // parse nested struct
            HZCC_CHECK_OR_ASSIGN(inner_struct,                    // NOLINT
                                 parse_struct(context, tokens));  // NOLINT;
            fields.push_back(std::move(inner_struct));
        }

        // consume type
        else {
            const auto field_type = tokens.peek();
            HZCC_CHECK_OR_ASSIGN(base_type, utils::get_base_type(tokens, context));
            if (tokens.empty())
                return utils::TokenErr(field_type,
                                       "Expected field name before end of input");
            for (;;) {

                // parse variable declaration
                HZCC_CHECK_OR_ASSIGN(
                    field, parse_type(tokens, context, base_type, context->at_root()));
                const auto& name = field.second;
                fields.push_back(std::make_unique<ast::FieldDecl>(
                    name.loc(), name.to_str(),
                    std::make_unique<ast::TypeProxyExpr>(name.loc(), field.first)));
                if (tokens.empty())
                    return utils::TokenErr(
                        name, "Expected ';' after struct field before end of input");

                // struct cannot have initializer
                if (tokens.peek().type() == TokenType::Assign) {
                    return utils::TokenErr(tokens, "Struct cannot have initializer");
                }

                // if define multiple value, we need to consume ','
                if (tokens.peek().type() != TokenType::Comma) break;
                const auto comma = tokens.pop();
                if (tokens.empty())
                    return utils::TokenErr(
                        comma, "Expected field name after ',' before end of input");
            }

            // consume ;
            HZCC_CheckAndConsume_ReturnErr(TokenType::SemiColon, tokens);
        }
    }

    // consume }
    HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::RBrace, tokens, struct_name));
    HZCC_CHECK_OK_OR_RETURN(utils::Consume(TokenType::SemiColon, tokens, start));

    // add variable to struct node
    for (auto& field : fields) struct_node->add_field(std::move(field));
    std::move(cancel_definition).Cancel();
    record_type->end_definition(true);
    return struct_node;
}
#pragma clang diagnostic pop
}  // namespace hzcc::syntax::parser