//
// Created by chen_ on 2022/5/14.
//
#include "options.h"

#ifndef FCALL3_CLASS_SOURCE_SYNTAX_PARSER_UTILS_MACROS_H_
#define FCALL3_CLASS_SOURCE_SYNTAX_PARSER_UTILS_MACROS_H_

#define HZCC_NODE_DEBUG_PRINT(node)                                 \
    "[" << (node)->NodeName() << ":" << (node)->GetNodeId() << "](" \
        << (node)->GetType()->GetName() << ")"

#define HZCC_CHECK_TYPE_OR_RETURN_NULLPTR(token, expr_l, expr_r, not_void,     \
                                          not_array)                           \
    {                                                                          \
        static_assert(                                                         \
            std::is_same_v<std::unique_ptr<AST::ASTNode>, decltype(expr_l)>,   \
            "Macro type mismatch, expr_l has to have a type of  "              \
            "'std::unique_ptr<ast::Stmt>'");                                   \
        static_assert(                                                         \
            std::is_same_v<std::unique_ptr<AST::ASTNode>, decltype(expr_r)>,   \
            "Macro type mismatch, expr_r has to have a type of  "              \
            "'std::unique_ptr<ast::Stmt>'");                                   \
        static_assert(                                                         \
            std::is_same_v<Lexical::Token, decltype(token)> ||                 \
                std::is_same_v<Lexical::Token&, decltype(token)> ||            \
                std::is_same_v<const Lexical::Token, decltype(token)> ||       \
                std::is_same_v<const Lexical::Token&, decltype(token)>,        \
            "Macro type mismatch, need lexical::Token for 1st variable");      \
        if (not_void &&                                                        \
            (lhs->GetType()->IsVoid() || rhs->GetType()->IsVoid())) {          \
            HZCC_PrintTokenError_ReturnErr(                                    \
                (token), "Expression does not allow have a type of void")      \
        }                                                                      \
                                                                               \
        if (not_array &&                                                       \
            (lhs->GetType()->IsArray() || rhs->GetType()->IsArray())) {        \
            HZCC_PrintTokenError_ReturnErr(                                    \
                (token), "Expression can not accept pointer")                  \
        }                                                                      \
                                                                               \
        if (!(expr_l)->GetType()->IsSame((expr_r)->GetType())) {               \
            DVLOG(SYNTAX_LOG_LEVEL)                                            \
                << "Fount type mis-match for lhs_expr: "                       \
                << HZCC_NODE_DEBUG_PRINT(expr_l)                               \
                << " rhs_expr: " << HZCC_NODE_DEBUG_PRINT(expr_r);             \
            auto expr_l_type = (expr_l)->GetType();                            \
            (expr_r) = AST::ASTNode::CastTo(expr_l_type, std::move((expr_r))); \
            if ((expr_r) == nullptr) {                                         \
                HZCC_PrintTokenError_ReturnErr(                                \
                    (token), "Expect type " + (expr_l)->GetType()->GetName() + \
                                 " but get " + expr_l_type->GetName());        \
            }                                                                  \
        }                                                                      \
    }

#endif  // FCALL3_CLASS_SOURCE_SYNTAX_PARSER_UTILS_MACROS_H_
