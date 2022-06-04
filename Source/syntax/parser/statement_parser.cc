//
// Created by chen_ on 2022/3/24.
//
#include "statement_parser.h"

#include <list>

#include "AST/CompilationUnit.h"
#include "AST/expr/AccessExpr.h"
#include "AST/expr/ArraySubscriptExpr.h"
#include "AST/expr/CastExpr.h"
#include "AST/expr/LiteralExpr.h"
#include "AST/expr/SizeofExpr.h"
#include "AST/expr/TernaryExpr.h"
#include "AST/expr/UnaryExpr.h"
#include "AST/expr/op/ArithmeticExpr.h"
#include "AST/expr/op/AssignExpr.h"
#include "AST/expr/op/BitwiseExpr.h"
#include "AST/expr/op/CommaExpr.h"
#include "AST/expr/op/LogicalExpr.h"
#include "AST/expr/op/RelationalExpr.h"
#include "AST/stmt/BreakStmt.h"
#include "AST/stmt/ContinueStmt.h"
#include "AST/stmt/DoStmt.h"
#include "AST/stmt/EmptyStmt.h"
#include "AST/stmt/ForStmt.h"
#include "AST/stmt/FuncCallStmt.h"
#include "AST/stmt/IfStmt.h"
#include "AST/stmt/ReturnStmt.h"
#include "AST/stmt/StructDeclStmt.h"
#include "AST/stmt/VarDecl.h"
#include "AST/type/ArrayType.h"
#include "lexical/Token.h"
#include "lexical/token_type.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"

namespace Hzcc::Syntax::Parser {
Statement::Statement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::ASTNode>(),
                 TypeNameUtil::name_pretty<AST::ASTNode>()) {}

std::unique_ptr<AST::ASTNode> Statement::parse_impl(TokenList& tokens,
                                                    SyntaxContext& context) {
    std::unique_ptr<AST::ASTNode> node{nullptr};
    switch (tokens.peek().Type()) {
        case Lexical::TokenType::kSemiColon:
            node =
                std::make_unique<AST::EmptyStatement>(tokens.peek().Location());
            break;
        case Lexical::TokenType::kStruct:
            if (tokens.peek3().Type() == Lexical::TokenType::kLBrace) {
                node = ParserFactory::ParseAST<AST::StructDeclStmt>(tokens,
                                                                    context);
            } else {
                node = ParseCommaExpr(context, tokens);
            }
            break;
        case Lexical::TokenType::kReturn: {
            auto token = tokens.pop();
            if (tokens.peek().Type() != Lexical::TokenType::kSemiColon) {
                auto ret_expr = ParseCommaExpr(context, tokens);
                if (ret_expr) {
                    if (context.AtRoot()) {
                        MYCC_PrintTokenError_ReturnNull(
                            token,
                            "return stmt is not allowed in the root "
                            "scope");
                    } else if (*ret_expr->RetType() ==
                               *context.GetReturnType()) {
                        auto rhs_type = ret_expr->RetType();
                        ret_expr = AST::ASTNode::CastTo(context.GetReturnType(),
                                                        std::move(ret_expr));
                        if (ret_expr == nullptr) {
                            MYCC_PrintTokenError_ReturnNull(
                                token,
                                "return stmt type is not match, "
                                "require: " +
                                    context.GetReturnType()->GetName() +
                                    ", got: " + rhs_type->GetName());
                        }
                    }

                    node = std::make_unique<AST::ReturnStmt>(
                        std::move(ret_expr), token.Location());
                } else {
                    VLOG(SYNTAX_LOG_LEVEL) << "parse return stmt error";
                    return nullptr;
                }
            } else {
                DLOG(INFO) << "ignore ';' since return stmt is empty";
                node = std::make_unique<AST::ReturnStmt>(
                    std::make_unique<AST::EmptyStatement>(token.Location()),
                    token.Location());
            }
            break;
        }
        case Lexical::TokenType::kIf:
            node = ParserFactory::ParseAST<AST::IfStmt>(tokens, context);
            break;
        case Lexical::TokenType::kDo:
            node = ParserFactory::ParseAST<AST::DoStatement>(tokens, context);
            break;
        case Lexical::TokenType::kFor:
            node = ParserFactory::ParseAST<AST::ForStmt>(tokens, context);
            break;
        case Lexical::TokenType::kWhile:
            node = ParserFactory::ParseAST<AST::WhileStmt>(tokens, context);
            break;
        case Lexical::TokenType::kBreak:
            if (WithinLoop()) {
                node =
                    std::make_unique<AST::BreakStmt>(tokens.peek().Location());
                tokens.pop();
            } else {
                MYCC_PrintFirstTokenError_ReturnNull(
                    tokens, "break stmt not within a loop");
            }
            break;
        case Lexical::TokenType::kContinue:
            if (WithinLoop()) {
                node = std::make_unique<AST::ContinueStmt>(
                    tokens.peek().Location());
                tokens.pop();
            } else {
                MYCC_PrintFirstTokenError_ReturnNull(
                    tokens, "continue stmt not within a loop");
            }
            break;
        case Lexical::TokenType::kType:
        case Lexical::TokenType::kIdentity: {
            auto type = Parser::ParseTypeName(tokens, context);
            if (context.hasType(Parser::TokenListToString(type))) {
                tokens.insert_front(type.begin(), type.end());
                node = ParserFactory::ParseAST<AST::VarDecl>(tokens, context);
                break;
            } else {
                tokens.insert_front(type.begin(), type.end());
            }
        }
        default:
            node = ParseCommaExpr(context, tokens);
            break;
    }

    if (node == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "stmt is empty";
    }
    return node;
}

std::unique_ptr<AST::ASTNode> Statement::ParseCommaExpr(SyntaxContext& context,
                                                        TokenList& tokens) {
    // parsing left-hand side of expression
    auto lhs = ParseAssignExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for comma expression failed";
        return nullptr;
    }

    // if we have a comma, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == Lexical::TokenType::kComma) {
        tokens.pop();
        auto rhs = ParseCommaExpr(context, tokens);
        if (lhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for comma expression failed";
            return nullptr;
        }

        return std::make_unique<AST::CommaExpr>(next.Location(), next.Value(),
                                                std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseAssignExpr(SyntaxContext& context,
                                                         TokenList& tokens) {
    auto lhs = ParseConditionalExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for assign expression failed";
        return nullptr;
    }

    // if we have a assign expr, we should parse the right-hand side
    if (tokens.peek().Type() == Lexical::TokenType::kAssign ||
        tokens.peek().Type() == Lexical::TokenType::kAddAssign ||
        tokens.peek().Type() == Lexical::TokenType::kSubAssign ||
        tokens.peek().Type() == Lexical::TokenType::kMulAssign ||
        tokens.peek().Type() == Lexical::TokenType::kDivAssign ||
        tokens.peek().Type() == Lexical::TokenType::kModAssign ||
        tokens.peek().Type() == Lexical::TokenType::kLShiftAssign ||
        tokens.peek().Type() == Lexical::TokenType::kRShiftAssign ||
        tokens.peek().Type() == Lexical::TokenType::kAndAssign ||
        tokens.peek().Type() == Lexical::TokenType::kXorAssign ||
        tokens.peek().Type() == Lexical::TokenType::kOrAssign) {
        auto assign_type = tokens.peek();
        tokens.pop();

        // parse rhs expression
        auto rhs = ParseAssignExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for assign expression failed";
            return nullptr;
        }

        // LHS has to be an assignable node
        if (!lhs->IsReturnLValue()) {
            MYCC_PrintTokenError_ReturnNull(assign_type,
                                            "Left-hand side is not assignable")
        }

        // LHS cannot be const variable
        if (lhs->RetType()->IsConst()) {
            VLOG(SYNTAX_LOG_LEVEL) << "LHS：" << lhs->Dump("");
            MYCC_PrintTokenError_ReturnNull(assign_type,
                                            "Left hand side is not assignable")
        }

        // lhs and rhs should not be arrayed
        if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
            MYCC_PrintTokenError_ReturnNull(
                assign_type, "No match for binary operation " +
                                 lhs->RetType()->GetName() + " " +
                                 Lexical::SymbolUtils::TokenTypeToString(
                                     assign_type.Type()) +
                                 " " + rhs->RetType()->GetName())
        }

        if (!(*lhs->RetType() == *rhs->RetType())) {
            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    assign_type, "Assignment mismatch " +
                                     lhs->RetType()->GetName() +
                                     " += " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::AssignExpr>(
            assign_type.Location(), assign_type.Value(), std::move(lhs),
            std::move(rhs));
    } else {
        return lhs;
    }
}

// TODO: add parser for conditional expression
std::unique_ptr<AST::ASTNode> Statement::ParseConditionalExpr(
    SyntaxContext& context, TokenList& tokens) {
    auto lhs = ParseLogicalOrExpr(context, tokens);

    if (tokens.peek().Type() == Lexical::TokenType::kQuestionMark) {
        if (lhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [LHS] for conditional expression failed";
            return nullptr;
        }

        auto cond_loc = tokens.peek().Location();
        tokens.pop();  // pop '?'

        // parse true expression

        // todo only allow char
        auto mhs = ParseConditionalExpr(context, tokens);
        if (mhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [MHS] for conditional expression failed";
            return nullptr;
        }

        // pop ':'
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kColon, tokens);

        // parse false expression
        auto next = tokens.peek();
        auto rhs = ParseConditionalExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for conditional expression failed";
            return nullptr;
        }

        if (!(*mhs->RetType() == *rhs->RetType())) {
            auto rhs_type = mhs->RetType();
            rhs = AST::ASTNode::CastTo(mhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::TernaryExpr>(
            cond_loc, std::move(lhs), std::move(mhs), std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseLogicalOrExpr(
    SyntaxContext& context, TokenList& tokens) {
    auto lhs = ParseLogicalAndExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for logical or expression failed";
        return nullptr;
    }

    // if we have a logical or, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == Lexical::TokenType::kLogicalOr) {
        tokens.pop();
        auto rhs = ParseLogicalOrExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for logical or expression failed";
            return nullptr;
        }

        // todo only allow char
        if (!(*lhs->RetType() == *rhs->RetType())) {
            // LHS and rhs should not be void
            if (lhs->RetType()->GetName(true) == "void" ||
                rhs->RetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation " +
                        lhs->RetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " " + rhs->RetType()->GetName())
            }

            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::LogicalExpr>(
            next.Value(), std::move(lhs), std::move(rhs), next.Location());
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseLogicalAndExpr(
    SyntaxContext& context, TokenList& tokens) {
    auto lhs = ParseBitwiseOrExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for logical and expression failed";
        return nullptr;
    }

    // if we have a logical and, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == Lexical::TokenType::kLogicalAnd) {
        tokens.pop();
        auto rhs = ParseLogicalAndExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for logical and expression failed";
            return nullptr;
        }

        // todo only allow char
        if (!(*lhs->RetType() == *rhs->RetType())) {
            // LHS and rhs should not be void
            if (lhs->RetType()->GetName(true) == "void" ||
                rhs->RetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation " +
                        lhs->RetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " " + rhs->RetType()->GetName())
            }

            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::LogicalExpr>(
            next.Value(), std::move(lhs), std::move(rhs), next.Location());
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseBitwiseOrExpr(
    SyntaxContext& context, TokenList& tokens) {
    auto lhs = ParseBitwiseXorExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for bitwise or expression failed";
        return nullptr;
    }

    // if we have a bitwise or, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == Lexical::TokenType::kBitwiseOr) {
        tokens.pop();
        auto rhs = ParseBitwiseOrExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for bitwise or expression failed";
            return nullptr;
        }

        if (!(*lhs->RetType() == *rhs->RetType())) {
            // LHS and rhs should not be void
            if (lhs->RetType()->GetName(true) == "void" ||
                rhs->RetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation " +
                        lhs->RetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " " + rhs->RetType()->GetName())
            }

            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::BitwiseExpr>(
            next.Location(), next.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseBitwiseXorExpr(
    SyntaxContext& context, TokenList& tokens) {
    auto lhs = ParseBitwiseAndExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for bitwise xor expression failed";
        return nullptr;
    }

    // if we have a bitwise xor, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == Lexical::TokenType::kBitwiseXor) {
        tokens.pop();
        auto rhs = ParseBitwiseXorExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for bitwise xor expression failed";
            return nullptr;
        }

        if (!(*lhs->RetType() == *rhs->RetType())) {
            // LHS and rhs should not be void
            if (lhs->RetType()->GetName(true) == "void" ||
                rhs->RetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation " +
                        lhs->RetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " " + rhs->RetType()->GetName())
            }

            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::BitwiseExpr>(
            next.Location(), next.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseBitwiseAndExpr(
    SyntaxContext& context, TokenList& tokens) {
    auto lhs = ParseEqualityExpr(context, tokens);

    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for bitwise and expression failed";
        return nullptr;
    }

    // if we have a bitwise and, we should parse the next expression
    auto next = tokens.peek();
    if (tokens.peek().Type() == Lexical::TokenType::kBitwiseAnd) {
        tokens.pop();
        auto rhs = ParseBitwiseAndExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for bitwise and expression failed";
            return nullptr;
        }

        if (!(*lhs->RetType() == *rhs->RetType())) {
            // LHS and rhs should not be void
            if (lhs->RetType()->GetName(true) == "void" ||
                rhs->RetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation " +
                        lhs->RetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " " + rhs->RetType()->GetName())
            }

            // trying to cast rhs to lhs
            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }
        return std::make_unique<AST::BitwiseExpr>(
            next.Location(), next.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseEqualityExpr(
    SyntaxContext& context, TokenList& tokens) {
    auto lhs = ParseRelationalExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for equality expression failed";
        return nullptr;
    }

    // if we have a equality, we should parse the next expression
    if (tokens.peek().Type() == Lexical::TokenType::kEqual ||
        tokens.peek().Type() == Lexical::TokenType::kNotEqual) {
        auto type = tokens.peek();
        tokens.pop();
        auto rhs = ParseEqualityExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for equality expression failed";
            return nullptr;
        }

        if (!(*lhs->RetType() == *rhs->RetType())) {
            // LHS and rhs should not be void
            if (lhs->RetType()->GetName(true) == "void" ||
                rhs->RetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation " +
                        lhs->RetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " " + rhs->RetType()->GetName())
            }

            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    type, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::RelationalExpr>(
            type.Location(), type.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseRelationalExpr(
    SyntaxContext& context, TokenList& tokens) {
    auto lhs = ParseShiftExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for relational expression failed";
        return nullptr;
    }

    // if we have a relational, we should parse the next
    // expression
    if (tokens.peek().Type() == Lexical::TokenType::kLess ||
        tokens.peek().Type() == Lexical::TokenType::kGreater ||
        tokens.peek().Type() == Lexical::TokenType::kLessEqual ||
        tokens.peek().Type() == Lexical::TokenType::kGreaterEqual) {
        auto type = tokens.peek();
        tokens.pop();
        auto rhs = ParseRelationalExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for relational "
                                      "expression failed";
            return nullptr;
        }

        if (!(*lhs->RetType() == *rhs->RetType())) {
            // LHS and rhs should not be void
            if (lhs->RetType()->GetName(true) == "void" ||
                rhs->RetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation " +
                        lhs->RetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " " + rhs->RetType()->GetName())
            }

            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    type, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::RelationalExpr>(
            type.Location(), type.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
    return nullptr;
}

std::unique_ptr<AST::ASTNode> Statement::ParseShiftExpr(SyntaxContext& context,
                                                        TokenList& tokens) {
    auto lhs = ParseAdditiveExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for shift expression failed";
        return nullptr;
    }

    // if we have a shift, we should parse the next expression
    if (tokens.peek().Type() == Lexical::TokenType::kLeftShift ||
        tokens.peek().Type() == Lexical::TokenType::kRightShift) {
        auto type = tokens.peek();
        tokens.pop();
        auto rhs = ParseShiftExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for shift expression failed";
            return nullptr;
        }

        if (!(*lhs->RetType() == *rhs->RetType())) {
            // LHS and rhs should not be void
            if (lhs->RetType()->GetName(true) == "void" ||
                rhs->RetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation " +
                        lhs->RetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " " + rhs->RetType()->GetName())
            }

            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    type, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::BitwiseExpr>(
            type.Location(), type.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseAdditiveExpr(
    SyntaxContext& context, TokenList& tokens) {
    auto lhs = ParseMultiplicativeExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for additive expression failed";
        return nullptr;
    }

    // if we have a additive, we should parse the next
    // expression
    if (tokens.peek().Type() == Lexical::TokenType::kAdd ||
        tokens.peek().Type() == Lexical::TokenType::kSub) {
        auto type = tokens.peek();
        tokens.pop();
        auto rhs = ParseAdditiveExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for additive expression failed";
            return nullptr;
        }

        // LHS and rhs should not be void
        if (lhs->RetType()->GetName(true) == "void" ||
            rhs->RetType()->GetName() == "void") {
            MYCC_PrintTokenError_ReturnNull(
                type, "No match for binary operation void " +
                          Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                          " void")
        }

        // lhs and rhs should not be arrayed
        if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
            MYCC_PrintTokenError_ReturnNull(
                type, "No match for binary operation " +
                          lhs->RetType()->GetName() + " " +
                          Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                          " " + rhs->RetType()->GetName())
        }

        // LHS == RHS
        if (!(*lhs->RetType() == *rhs->RetType())) {
            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    type, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::ArithmeticExpr>(
            type.Location(), type.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
    return nullptr;
}

std::unique_ptr<AST::ASTNode> Statement::ParseMultiplicativeExpr(
    SyntaxContext& context, TokenList& tokens) {
    auto lhs = ParseUnaryExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for multiplicative "
                                  "expression failed";
        return nullptr;
    }

    // if we have a multiplicative, we should parse the next
    // expression
    if (tokens.peek().Type() == Lexical::TokenType::kMul ||
        tokens.peek().Type() == Lexical::TokenType::kDiv ||
        tokens.peek().Type() == Lexical::TokenType::kMod) {
        auto type = tokens.peek();
        tokens.pop();
        auto rhs = ParseMultiplicativeExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for multiplicative "
                                      "expression failed";
            return nullptr;
        }

        // LHS and rhs should not be void
        if (lhs->RetType()->GetName(true) == "void" ||
            rhs->RetType()->GetName() == "void") {
            MYCC_PrintTokenError_ReturnNull(
                type, "No match for binary operation void " +
                          Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                          " void")
        }

        // lhs and rhs should not be arrayed
        if (lhs->RetType()->IsArray() || rhs->RetType()->IsArray()) {
            MYCC_PrintTokenError_ReturnNull(
                type, "No match for binary operation " +
                          lhs->RetType()->GetName() + " " +
                          Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                          " " + rhs->RetType()->GetName())
        }

        // LHS == RHS
        if (!(*lhs->RetType() == *rhs->RetType())) {
            auto rhs_type = rhs->RetType();
            rhs = AST::ASTNode::CastTo(lhs->RetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    type, "Expect type " + lhs->RetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::ArithmeticExpr>(
            type.Location(), type.Value(), std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseUnaryExpr(SyntaxContext& context,
                                                        TokenList& tokens) {
    // some of unary expression does not have LHS
    if (tokens.peek().Type() == Lexical::TokenType::kSub ||
        tokens.peek().Type() == Lexical::TokenType::kLogicalNot ||
        tokens.peek().Type() == Lexical::TokenType::kReference ||
        tokens.peek().Type() == Lexical::TokenType::kBitWiseNot ||
        tokens.peek().Type() == Lexical::TokenType::kDereference) {
        auto type = tokens.peek();
        tokens.pop();
        auto rhs = ParseUnaryExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for unary expression failed";
            return nullptr;
        }

        return std::make_unique<AST::UnaryExpr>(type.Location(), type.Value(),
                                                std::move(rhs));
    }

    // for cast expression, we should parse the next expression
    else if (tokens.peek().Type() == Lexical::TokenType::kLParentheses &&
             tokens.size() >= 2 && context.hasType(tokens.peek2().Value())) {
        auto cast_loc = tokens.peek();
        tokens.pop();  // consume the '('

        // get the type of the cast expression
        auto type_name = ParseTypeName(tokens, context);

        if (type_name.empty()) {
            MYCC_PrintTokenError(tokens.peek(), "Expected type name");
            return nullptr;
        }

        auto [argument_type, name_token] =
            ParseTypeDecl(TokenListToString(type_name), tokens, context);

        // consume the ')'
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses,
                                        tokens);

        auto rhs = ParseUnaryExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for cast expression failed";
            return nullptr;
        }

        // RetType == RHS
        if ((argument_type->IsArray() && !rhs->RetType()->IsArray()) ||
            (!argument_type->IsArray() && rhs->RetType()->IsArray()) ||
            (argument_type->GetName() == "void" ||
             rhs->RetType()->GetName() == "void")) {
            MYCC_PrintTokenError_ReturnNull(
                cast_loc, "Cannot cast expression of type " +
                              rhs->RetType()->GetName() + " to " +
                              argument_type->GetName());
        }

        if (rhs->RetType()->IsConst()) {
            return std::make_unique<AST::CastExpr>(
                cast_loc.Location(), argument_type->GetConstType(),
                std::move(rhs));
        } else {
            return std::make_unique<AST::CastExpr>(
                cast_loc.Location(), argument_type, std::move(rhs));
        }
    }

    // sizeof expr
    else if (tokens.peek().Type() == Lexical::TokenType::kSizeOf) {
        auto size_of_loc = tokens.peek().Location();
        tokens.pop();  // consume the sizeof

        // consume the '('
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses,
                                        tokens);

        auto expr = ParseUnaryExpr(context, tokens);
        if (expr == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [EXPR] for sizeof expression failed";
            return nullptr;
        }

        // consume the ')'
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses,
                                        tokens);

        return std::make_unique<AST::SizeofExpr>(std::move(expr), size_of_loc);
    }

    // selfInc/selfDec and otherwise
    else {
        // since self inc/dec could before the variable or after
        // the variable we need to check the next token
        if (tokens.peek().Type() == Lexical::TokenType::kSelfIncrement ||
            tokens.peek().Type() == Lexical::TokenType::kSelfDecrement) {
            auto type = tokens.peek();
            tokens.pop();

            // parsing RHS
            auto rhs = ParseUnaryExpr(context, tokens);
            if (rhs == nullptr) {
                VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for unary "
                                          "expression failed";
                return nullptr;
            }

            // LHS has to be a variable Node
            if (!rhs->IsReturnLValue()) {
                MYCC_PrintTokenError_ReturnNull(type,
                                                "cannot add value to rvalue");
            }

            // LHS has to be a variable Node
            if (rhs->RetType()->IsConst()) {
                MYCC_PrintTokenError_ReturnNull(
                    type, "cannot set value to const variable");
            }

            return std::make_unique<AST::UnaryExpr>(
                type.Location(), type.Value(), std::move(rhs));
        } else {
            auto lhs = ParseAccessExpr(context, tokens);
            if (lhs == nullptr) {
                VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for unary "
                                          "expression failed";
                return nullptr;
            }

            // check the next token
            if (tokens.peek().Type() == Lexical::TokenType::kSelfIncrement) {
                // LHS has to be a variable Node
                if (!lhs->IsReturnLValue()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "Expression is not assignable")
                }

                // LHS has to be a variable Node
                if (lhs->RetType()->IsConst()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "cannot set value to const variable")
                }

                auto type = tokens.pop();
                return std::make_unique<AST::UnaryExpr>(
                    type.Location(), type.Value(), std::move(lhs));
            } else if (tokens.peek().Type() ==
                       Lexical::TokenType::kSelfDecrement) {
                // LHS has to be a variable Node
                if (!lhs->IsReturnLValue()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "Expression is not assignable")
                }

                // LHS has to be a variable Node
                if (lhs->RetType()->IsConst()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "cannot set value to const variable");
                }
                auto type = tokens.pop();
                return std::make_unique<AST::UnaryExpr>(
                    type.Location(), type.Value(), std::move(lhs));
            } else {
                return std::move(lhs);
            }
        }
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseAccessExpr(SyntaxContext& context,
                                                         TokenList& tokens) {
    auto currentExpr = ParsePostfixExpr(context, tokens);

    if (currentExpr == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [EXPR] for access expression failed";
        return nullptr;
    }

    // variable accessor
    if (tokens.peek().Type() == Lexical::TokenType::kDot ||
        tokens.peek().Type() == Lexical::TokenType::kArrow) {
        do {
            if (tokens.peek().Type() == Lexical::TokenType::kArrow ||
                tokens.peek().Type() == Lexical::TokenType::kDot) {
                // we need to know is it a pointer or not
                bool isPtr = tokens.pop().Type() == Lexical::TokenType::kArrow;

                // accessor should not be a keyword
                if (tokens.peek().Type() != Lexical::TokenType::kIdentity) {
                    MYCC_PrintFirstTokenError_ReturnNull(tokens,
                                                         "Expect identifier")
                }

                // to new a variable node
                auto memberName = tokens.pop();

                //  check if the type has this member

                if (!currentExpr->RetType()->IsStruct()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens,
                        "Expect struct type for member "
                        "access")
                }

                auto type =
                    dynamic_cast<AST::StructType*>(currentExpr->RetType().get())
                        ->GetChild(memberName.Value());

                // check member name
                if (type == nullptr) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "Unknown member '" + memberName.Value() +
                                    "' in " + currentExpr->RetType()->GetName())
                }

                currentExpr = std::make_unique<AST::AccessExpr>(
                    isPtr, memberName.Location(), memberName.Value(),
                    std::move(currentExpr));
            } else {
                // consume the ']'
                auto start = tokens.peek();
                MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLBracket,
                                                tokens);

                // consume the index
                auto indexExpr = ParseCommaExpr(context, tokens);
                if (indexExpr == nullptr) {
                    VLOG(SYNTAX_LOG_LEVEL) << "Parse [INDEX] for array "
                                              "accessor failed";
                    return nullptr;
                }

                // check type
                if (!currentExpr->RetType()->IsArray()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "Expect array type for array accessor")
                }

                // consume the ']'

                MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRBracket,
                                                tokens);
                currentExpr = std::make_unique<AST::ArraySubscriptExpr>(
                    start.Location(), std::move(currentExpr),
                    std::move(indexExpr));
            }
        } while (tokens.peek().Type() == Lexical::TokenType::kDot ||
                 tokens.peek().Type() == Lexical::TokenType::kArrow ||
                 tokens.peek().Type() == Lexical::TokenType::kLBracket);

        return currentExpr;
    } else {
        return currentExpr;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParsePostfixExpr(
    SyntaxContext& context, TokenList& tokens) {
    // first handle priority expression ("()")
    if (tokens.peek().Type() == Lexical::TokenType::kLParentheses) {
        tokens.pop();  // consume the '('

        auto expr = ParseCommaExpr(context, tokens);
        if (expr == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [EXPR] for postfix expression failed";
            return nullptr;
        }

        // consume the ')'
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses,
                                        tokens);

        return std::move(expr);
    }

    // handle access/array
    else if (tokens.peek().Type() == Lexical::TokenType::kIdentity &&
             !context.hasType(tokens.peek().Value())) {
        auto name = tokens.pop();

        // array
        if (tokens.peek().Type() == Lexical::TokenType::kLBracket) {
            auto start = tokens.pop();  // consume the '['

            // array accessor should be integer
            auto index = ParseCommaExpr(context, tokens);
            if (index == nullptr) {
                VLOG(SYNTAX_LOG_LEVEL) << "Parse [INDEX] for array "
                                          "accessor failed";
                return nullptr;
            }

            auto int_type = AST::Type::GetTypeOf("int", {});
            if (!(*int_type == *index->RetType())) {
                auto index_type = index->RetType();
                index = AST::ASTNode::CastTo(int_type, std::move(index));
                if (index == nullptr) {
                    MYCC_PrintTokenError_ReturnNull(
                        start,
                        "Expect type int but get " + index_type->GetName());
                }
            }

            // consume the ']'
            MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRBracket,
                                            tokens);

            if (!context.hasVariable(name.Value(), false)) {
                MYCC_PrintTokenError_ReturnNull(
                    name, "Undefined variable: " + name.Value());
            }

            auto type = context.getVariableType(name.Value());

            if (type->IsArray()) {
                return std::make_unique<AST::ArraySubscriptExpr>(
                    start.Location(),
                    std::make_unique<AST::DeclRefExpr>(name.Location(), type,
                                                       name.Value()),
                    std::move(index));
            } else {
                MYCC_PrintTokenError_ReturnNull(
                    name, "Expect array type for array accessor");
            }
        }

        // function caller
        else if (tokens.peek().Type() == Lexical::TokenType::kLParentheses) {
            tokens.pop();  // consume the '('

            // check function name is matched
            if (!context.hasFunction(name.Value())) {
                MYCC_PrintTokenError_ReturnNull(
                    name, "Function " + name.Value() + " not found");
            }

            // next token should not be ", "
            if (tokens.peek().Type() == Lexical::TokenType::kComma) {
                MYCC_PrintFirstTokenError_ReturnNull(tokens, "term expected");
            }

            // parse arguments
            std::list<std::unique_ptr<AST::ASTNode>> args_expr;
            while (tokens.peek().Type() != Lexical::TokenType::kRParentheses) {
                auto arg_token = tokens.peek();
                auto arg = ParseAssignExpr(context, tokens);
                if (arg == nullptr) {
                    VLOG(SYNTAX_LOG_LEVEL) << "Parse [ARG] for function "
                                              "caller failed";
                    return nullptr;
                }

                // consume the ',' if have other arguments
                if (tokens.peek().Type() != Lexical::TokenType::kRParentheses) {
                    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kComma,
                                                    tokens);

                    // after consume the ',', we should have a
                    // valid argument
                    if (tokens.peek().Type() ==
                        Lexical::TokenType::kRParentheses) {
                        MYCC_PrintTokenError_ReturnNull(tokens.peek(),
                                                        "term expected");
                    }
                }
                args_expr.push_back(std::move(arg));
            }

            // get function return type and argument types
            auto [return_type, funcType, line_no] =
                context.getFuncRetAndArgType(name.Value());

            // function call need to have same number of arguments

            if (!context.hasFunction(name.Value())) {
                MYCC_PrintTokenError_ReturnNull(
                    name, "Function " + name.Value() + " not found");
            }

            // check number of arguments
            if (funcType.size() != args_expr.size()) {
                // generate type string first
                std::string type_str = "(";
                for (const auto& arg : funcType) {
                    type_str += arg->GetName() + ", ";
                }
                type_str.replace(type_str.end() - 2, type_str.end(), ")");

                // print out error message
                MYCC_PrintTokenError_ReturnNull(
                    name, "Parameter mismatch in call to " +
                              return_type->GetName() + " " + name.Value() +
                              type_str + "\n\tExpected " +
                              std::to_string(funcType.size()) + ", received " +
                              std::to_string(args_expr.size()) + " parameters");
            }

            // check argument types for all arguments
            for (auto& expr : args_expr) {
                if (!(*expr->RetType() == *funcType.front())) {
                    auto arg_type = expr->RetType();
                    expr =
                        AST::ASTNode::CastTo(funcType.front(), std::move(expr));
                    if (expr == nullptr) {
                        MYCC_PrintTokenError_ReturnNull(
                            name, "Expect type " + funcType.front()->GetName() +
                                      " for argument " +
                                      std::to_string(args_expr.size() + 1) +
                                      " but get " + arg_type->GetName());
                    }
                }
                funcType.pop_front();
            }

            // consume the ')'
            MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses,
                                            tokens);

            return std::make_unique<AST::FuncCallStmt>(
                name.Location(), return_type, name.Value(),
                std::move(args_expr));
        }

        if (context.hasVariable(name.Value(), false)) {
            return std::make_unique<AST::DeclRefExpr>(
                name.Location(), context.getVariableType(name.Value()),
                name.Value());
        } else {
            MYCC_PrintTokenError_ReturnNull(
                name, "Undefined variable: " + name.Value());
        }
    }

    // const values (string literal will handle differently
    // since it could be concat)
    else if (tokens.peek().Type() == Lexical::TokenType::kChar ||
             tokens.peek().Type() == Lexical::TokenType::kInteger ||
             tokens.peek().Type() == Lexical::TokenType::kReal_number) {
        auto value = tokens.peek();

        switch (tokens.pop().Type()) {
            case Lexical::TokenType::kChar:
                return std::make_unique<AST::LiteralExpr>(
                    AST::kLiteralType_Char, value.Value(), value.Location());
            case Lexical::TokenType::kInteger:
                return std::make_unique<AST::LiteralExpr>(
                    AST::kLiteralType_Integer, value.Value(), value.Location());
            case Lexical::TokenType::kReal_number:
                return std::make_unique<AST::LiteralExpr>(
                    AST::kLiteralType_Real_number, value.Value(),
                    value.Location());
            default:
                LOG(FATAL) << "Unreachable code: \n"
                           << "\t Current AST:\n";
                return nullptr;
        }
    }

    // handle string literal
    else if (tokens.peek().Type() == Lexical::TokenType::kString) {
        auto value = tokens.pop();
        auto value_str = value.Value();

        while (tokens.peek().Type() == Lexical::TokenType::kString) {
            value_str += tokens.pop().Value();
        }

        return std::make_unique<AST::LiteralExpr>(AST::kLiteralType_String,
                                                  value_str, value.Location());
    }

    // if we can detect types
    else {
        if (tokens.peek().Type() == Lexical::TokenType::kElse) {
            MYCC_PrintFirstTokenError_ReturnNull(
                tokens, "Else has to be followed by if");
        } else {
            MYCC_PrintFirstTokenError_ReturnNull(tokens, "term expected")
        }
    }
}

}  // namespace Hzcc::Syntax::Parser
