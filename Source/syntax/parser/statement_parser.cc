//
// Created by chen_ on 2022/3/24.
//
#include "statement_parser.h"

#include <list>

#include "AST/CompilationUnit.h"
#include "AST/expr/access.h"
#include "AST/expr/array.h"
#include "AST/expr/cast.h"
#include "AST/expr/conditional.h"
#include "AST/expr/literal.h"
#include "AST/expr/operator/arithmetic.h"
#include "AST/expr/operator/assign.h"
#include "AST/expr/operator/bitwise.h"
#include "AST/expr/operator/comma.h"
#include "AST/expr/operator/logical.h"
#include "AST/expr/operator/relational.h"
#include "AST/expr/sizeof.h"
#include "AST/expr/unary.h"
#include "AST/statement/break.h"
#include "AST/statement/continue.h"
#include "AST/statement/do.h"
#include "AST/statement/empty.h"
#include "AST/statement/for.h"
#include "AST/statement/function_call.h"
#include "AST/statement/if.h"
#include "AST/statement/return.h"
#include "AST/statement/struct.h"
#include "AST/statement/value_decl.h"
#include "AST/type/ArrayType.h"
#include "lexical/Token.h"
#include "lexical/token_type.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"

namespace Mycc::Syntax::Parser {
using namespace TokenListUtils;
Statement::Statement() noexcept
    : ParserBase(TypeNameUtil::hash<AST::ASTNode>(),
                 TypeNameUtil::name_pretty<AST::ASTNode>()) {}

std::unique_ptr<AST::ASTNode> Statement::parse_impl(AST::CompilationUnit& context,
                                                    TokenList& tokens,
                                                    TokenList& attributes) {
    ConcatAttribute(attributes, tokens);
    std::unique_ptr<AST::ASTNode> node{nullptr};
    switch (peek(tokens).Type()) {
        case Lexical::TokenType::kSemiColon:
            node = std::make_unique<AST::EmptyStatement>(
                tokens.front().Location());
            break;
        case Lexical::TokenType::kReturn: {
            auto token = pop_list(tokens);
            if (peek(tokens).Type() != Lexical::TokenType::kSemiColon) {
                auto ret_expr = ParseCommaExpr(context, tokens);
                if (ret_expr) {
                    if (Options::Global_enable_type_checking) {
                        if (context.AtRoot()) {
                            MYCC_PrintTokenError_ReturnNull(
                                token,
                                "return statement is not allowed in the root "
                                "scope");
                        } else if (!ret_expr->GetType()->IsSame(
                                       context.GetReturnType())) {
                            MYCC_PrintTokenError_ReturnNull(
                                token, "Return type mismatch: was " +
                                           ret_expr->GetType()->GetName() +
                                           ", expected " +
                                           context.GetReturnType()->GetName());
                        }
                    }

                    node = std::make_unique<AST::ReturnNode>(
                        token, std::move(ret_expr));
                } else {
                    VLOG(SYNTAX_LOG_LEVEL) << "parse return statement error";
                    return nullptr;
                }
            } else {
                DLOG(INFO) << "ignore ';' since return statement is empty";
                node = std::make_unique<AST::ReturnNode>(
                    token,
                    std::make_unique<AST::EmptyStatement>(token.Location()));
            }
            break;
        }
        case Lexical::TokenType::kIf:
            node = ParserFactory::ParseAST<AST::IfStatement>(context, tokens,
                                                             attributes);
            break;
        case Lexical::TokenType::kDo:
            node = ParserFactory::ParseAST<AST::DoStatement>(context, tokens,
                                                             attributes);
            break;
        case Lexical::TokenType::kFor:
            node = ParserFactory::ParseAST<AST::ForStatement>(context, tokens,
                                                              attributes);
            break;
        case Lexical::TokenType::kWhile:
            node = ParserFactory::ParseAST<AST::WhileStatement>(context, tokens,
                                                                attributes);
            break;
        case Lexical::TokenType::kBreak:
            node =
                std::make_unique<AST::BreakStatement>(peek(tokens).Location());
            pop_list(tokens);
            break;
        case Lexical::TokenType::kContinue:
            node = std::make_unique<AST::ContinueStatement>(
                peek(tokens).Location());
            pop_list(tokens);
            break;
        case Lexical::TokenType::kType:
        case Lexical::TokenType::kIdentity: {
            auto type = Parser::ParseTypeName(context, tokens);
            if (context.hasType(Parser::TokenListToString(type))) {
                tokens.insert(tokens.begin(), type.begin(), type.end());
                node = ParserFactory::ParseAST<AST::VarDecl>(context, tokens,
                                                             attributes);
                break;
            } else {
                tokens.insert(tokens.begin(), type.begin(), type.end());
            }
        }
        default:
            node = ParseCommaExpr(context, tokens);
            break;
    }

    if (node == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "statement is empty";
    }

    attributes.clear();
    return node;
}

std::unique_ptr<AST::ASTNode> Statement::ParseCommaExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    // parsing left-hand side of expression
    auto lhs = ParseAssignExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for comma expression failed";
        return nullptr;
    }

    // if we have a comma, we should parse the next expression
    auto next = peek(tokens);
    if (peek(tokens).Type() == Lexical::TokenType::kComma) {
        pop_list(tokens);
        auto rhs = ParseCommaExpr(context, tokens);
        if (lhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for comma expression failed";
            return nullptr;
        }

        return std::make_unique<AST::CommaExpr>(next, std::move(lhs),
                                                std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseAssignExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseConditionalExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for assign expression failed";
        return nullptr;
    }

    // if we have a assign expr, we should parse the right-hand side
    if (peek(tokens).Type() == Lexical::TokenType::kAssign ||
        peek(tokens).Type() == Lexical::TokenType::kAddAssign ||
        peek(tokens).Type() == Lexical::TokenType::kSubAssign ||
        peek(tokens).Type() == Lexical::TokenType::kMulAssign ||
        peek(tokens).Type() == Lexical::TokenType::kDivAssign ||
        peek(tokens).Type() == Lexical::TokenType::kModAssign ||
        peek(tokens).Type() == Lexical::TokenType::kLShiftAssign ||
        peek(tokens).Type() == Lexical::TokenType::kRShiftAssign ||
        peek(tokens).Type() == Lexical::TokenType::kAndAssign ||
        peek(tokens).Type() == Lexical::TokenType::kXorAssign ||
        peek(tokens).Type() == Lexical::TokenType::kOrAssign) {
        auto assign_type = tokens.front();
        pop_list(tokens);

        // parse rhs expression
        auto rhs = ParseAssignExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for assign expression failed";
            return nullptr;
        }

        // LHS has to be an assignable node
        if (!lhs->IsAssignable()) {
            MYCC_PrintTokenError_ReturnNull(assign_type,
                                            "Left-hand side is not assignable")
        }

        if (Options::Global_enable_type_checking) {
            Message::set_current_part("Type checking");
            // LHS cannot be const variable
            if (lhs->GetType()->IsConst()) {
                VLOG(SYNTAX_LOG_LEVEL) << "LHS：" << lhs->Dump("");
                MYCC_PrintTokenError_ReturnNull(
                    assign_type, "Left hand side is not assignable")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    assign_type, "No match for binary operation " +
                                     lhs->GetType()->GetName() + " " +
                                     Lexical::SymbolUtils::TokenTypeToString(
                                         assign_type.Type()) +
                                     " " + rhs->GetType()->GetName())
            }

            if (!lhs->GetType()->IsSame(rhs->GetType())) {
                auto rhs_type = rhs->GetType();
                rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
                if (rhs == nullptr) {
                    MYCC_PrintTokenError_ReturnNull(
                        assign_type, "Assignment mismatch " +
                                         lhs->GetType()->GetName() +
                                         " += " + rhs_type->GetName());
                }
            }
        }

        return std::make_unique<AST::AssignExpr>(assign_type, std::move(lhs),
                                                 std::move(rhs));
    } else {
        return lhs;
    }
}

// TODO: add parser for conditional expression
std::unique_ptr<AST::ASTNode> Statement::ParseConditionalExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseLogicalOrExpr(context, tokens);

    if (peek(tokens).Type() == Lexical::TokenType::kQuestionMark) {
        if (lhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [LHS] for conditional expression failed";
            return nullptr;
        }

        auto cond_loc = tokens.front().Location();
        pop_list(tokens);  // pop '?'

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
        auto next = tokens.front();
        auto rhs = ParseConditionalExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for conditional expression failed";
            return nullptr;
        }

        if (Options::Global_enable_type_checking &&
            !mhs->GetType()->IsSame(rhs->GetType())) {
            Message::set_current_part("Type checking");

            auto rhs_type = mhs->GetType();
            rhs = AST::ASTNode::CastTo(mhs->GetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->GetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }

            Message::set_current_part("Parser");
        }

        return std::make_unique<AST::ConditionalExpr>(
            std::move(lhs), std::move(mhs), std::move(rhs), cond_loc);
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseLogicalOrExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseLogicalAndExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for logical or expression failed";
        return nullptr;
    }

    // if we have a logical or, we should parse the next expression
    auto next = peek(tokens);
    if (peek(tokens).Type() == Lexical::TokenType::kLogicalOr) {
        pop_list(tokens);
        auto rhs = ParseLogicalOrExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for logical or expression failed";
            return nullptr;
        }

        // todo only allow char
        if (Options::Global_enable_type_checking &&
            !lhs->GetType()->IsSame(rhs->GetType())) {
            Message::set_current_part("Type checking");

            // LHS and rhs should not be void
            if (lhs->GetType()->GetName(true) == "void" ||
                rhs->GetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation " +
                        lhs->GetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " " + rhs->GetType()->GetName())
            }

            auto rhs_type = rhs->GetType();
            rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->GetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
            Message::set_current_part("Parser");
        }

        return std::make_unique<AST::LogicalExpr>(next, std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseLogicalAndExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseBitwiseOrExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for logical and expression failed";
        return nullptr;
    }

    // if we have a logical and, we should parse the next expression
    auto next = peek(tokens);
    if (peek(tokens).Type() == Lexical::TokenType::kLogicalAnd) {
        pop_list(tokens);
        auto rhs = ParseLogicalAndExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for logical and expression failed";
            return nullptr;
        }

        // todo only allow char
        if (Options::Global_enable_type_checking &&
            !lhs->GetType()->IsSame(rhs->GetType())) {
            Message::set_current_part("Type checking");

            // LHS and rhs should not be void
            if (lhs->GetType()->GetName(true) == "void" ||
                rhs->GetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation " +
                        lhs->GetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " " + rhs->GetType()->GetName())
            }

            auto rhs_type = rhs->GetType();
            rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->GetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }

            Message::set_current_part("Parser");
        }

        return std::make_unique<AST::LogicalExpr>(next, std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseBitwiseOrExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseBitwiseXorExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for bitwise or expression failed";
        return nullptr;
    }

    // if we have a bitwise or, we should parse the next expression
    auto next = peek(tokens);
    if (peek(tokens).Type() == Lexical::TokenType::kBitwiseOr) {
        pop_list(tokens);
        auto rhs = ParseBitwiseOrExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for bitwise or expression failed";
            return nullptr;
        }

        if (Options::Global_enable_type_checking &&
            !lhs->GetType()->IsSame(rhs->GetType())) {
            Message::set_current_part("Type checking");

            // LHS and rhs should not be void
            if (lhs->GetType()->GetName(true) == "void" ||
                rhs->GetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation " +
                        lhs->GetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " " + rhs->GetType()->GetName())
            }

            auto rhs_type = rhs->GetType();
            rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->GetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }

            Message::set_current_part("Parser");
        }

        return std::make_unique<AST::BitwiseExpr>(next, std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseBitwiseXorExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseBitwiseAndExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for bitwise xor expression failed";
        return nullptr;
    }

    // if we have a bitwise xor, we should parse the next expression
    auto next = peek(tokens);
    if (peek(tokens).Type() == Lexical::TokenType::kBitwiseXor) {
        pop_list(tokens);
        auto rhs = ParseBitwiseXorExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for bitwise xor expression failed";
            return nullptr;
        }

        if (Options::Global_enable_type_checking &&
            !lhs->GetType()->IsSame(rhs->GetType())) {
            Message::set_current_part("Type checking");

            // LHS and rhs should not be void
            if (lhs->GetType()->GetName(true) == "void" ||
                rhs->GetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation " +
                        lhs->GetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " " + rhs->GetType()->GetName())
            }

            auto rhs_type = rhs->GetType();
            rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->GetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
            Message::set_current_part("Parser");
        }

        return std::make_unique<AST::BitwiseExpr>(next, std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseBitwiseAndExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseEqualityExpr(context, tokens);

    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for bitwise and expression failed";
        return nullptr;
    }

    // if we have a bitwise and, we should parse the next expression
    auto next = peek(tokens);
    if (peek(tokens).Type() == Lexical::TokenType::kBitwiseAnd) {
        pop_list(tokens);
        auto rhs = ParseBitwiseAndExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for bitwise and expression failed";
            return nullptr;
        }

        if (Options::Global_enable_type_checking &&
            !lhs->GetType()->IsSame(rhs->GetType())) {
            Message::set_current_part("Type checking");

            // LHS and rhs should not be void
            if (lhs->GetType()->GetName(true) == "void" ||
                rhs->GetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    next,
                    "No match for binary operation " +
                        lhs->GetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(next.Type()) +
                        " " + rhs->GetType()->GetName())
            }

            // trying to cast rhs to lhs
            auto rhs_type = rhs->GetType();
            rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    next, "Expect type " + lhs->GetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
            Message::set_current_part("Parser");
        }
        return std::make_unique<AST::BitwiseExpr>(next, std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseEqualityExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseRelationalExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for equality expression failed";
        return nullptr;
    }

    // if we have a equality, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kEqual ||
        peek(tokens).Type() == Lexical::TokenType::kNotEqual) {
        auto type = peek(tokens);
        pop_list(tokens);
        auto rhs = ParseEqualityExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for equality expression failed";
            return nullptr;
        }

        if (Options::Global_enable_type_checking &&
            !lhs->GetType()->IsSame(rhs->GetType())) {
            Message::set_current_part("Type checking");

            // LHS and rhs should not be void
            if (lhs->GetType()->GetName(true) == "void" ||
                rhs->GetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation " +
                        lhs->GetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " " + rhs->GetType()->GetName())
            }

            auto rhs_type = rhs->GetType();
            rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    type, "Expect type " + lhs->GetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }

            Message::set_current_part("Parser");
        }

        return std::make_unique<AST::RelationalExpr>(type, std::move(lhs),
                                                     std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseRelationalExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseShiftExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL)
            << "Parse [LHS] for relational expression failed";
        return nullptr;
    }

    // if we have a relational, we should parse the next
    // expression
    if (peek(tokens).Type() == Lexical::TokenType::kLess ||
        peek(tokens).Type() == Lexical::TokenType::kGreater ||
        peek(tokens).Type() == Lexical::TokenType::kLessEqual ||
        peek(tokens).Type() == Lexical::TokenType::kGreaterEqual) {
        auto type = peek(tokens);
        pop_list(tokens);
        auto rhs = ParseRelationalExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for relational "
                                      "expression failed";
            return nullptr;
        }

        if (Options::Global_enable_type_checking &&
            !lhs->GetType()->IsSame(rhs->GetType())) {
            Message::set_current_part("Type checking");

            // LHS and rhs should not be void
            if (lhs->GetType()->GetName(true) == "void" ||
                rhs->GetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation " +
                        lhs->GetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " " + rhs->GetType()->GetName())
            }

            auto rhs_type = rhs->GetType();
            rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    type, "Expect type " + lhs->GetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
            Message::set_current_part("Parser");
        }

        return std::make_unique<AST::RelationalExpr>(type, std::move(lhs),
                                                     std::move(rhs));
    } else {
        return lhs;
    }
    return nullptr;
}

std::unique_ptr<AST::ASTNode> Statement::ParseShiftExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseAdditiveExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for shift expression failed";
        return nullptr;
    }

    // if we have a shift, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kLeftShift ||
        peek(tokens).Type() == Lexical::TokenType::kRightShift) {
        auto type = peek(tokens);
        pop_list(tokens);
        auto rhs = ParseShiftExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for shift expression failed";
            return nullptr;
        }

        if (Options::Global_enable_type_checking &&
            !lhs->GetType()->IsSame(rhs->GetType())) {
            Message::set_current_part("Type checking");

            // LHS and rhs should not be void
            if (lhs->GetType()->GetName(true) == "void" ||
                rhs->GetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation " +
                        lhs->GetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " " + rhs->GetType()->GetName())
            }

            auto rhs_type = rhs->GetType();
            rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
            if (rhs == nullptr) {
                MYCC_PrintTokenError_ReturnNull(
                    type, "Expect type " + lhs->GetType()->GetName() +
                              " but get " + rhs_type->GetName());
            }
        }

        return std::make_unique<AST::BitwiseExpr>(type, std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseAdditiveExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseMultiplicativeExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for additive expression failed";
        return nullptr;
    }

    // if we have a additive, we should parse the next
    // expression
    if (peek(tokens).Type() == Lexical::TokenType::kAdd ||
        peek(tokens).Type() == Lexical::TokenType::kSub) {
        auto type = peek(tokens);
        pop_list(tokens);
        auto rhs = ParseAdditiveExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL)
                << "Parse [RHS] for additive expression failed";
            return nullptr;
        }

        if (Options::Global_enable_type_checking) {
            Message::set_current_part("Type checking");

            // LHS and rhs should not be void
            if (lhs->GetType()->GetName(true) == "void" ||
                rhs->GetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation " +
                        lhs->GetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " " + rhs->GetType()->GetName())
            }

            // LHS == RHS
            if (!lhs->GetType()->IsSame(rhs->GetType())) {
                auto rhs_type = rhs->GetType();
                rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
                if (rhs == nullptr) {
                    MYCC_PrintTokenError_ReturnNull(
                        type, "Expect type " + lhs->GetType()->GetName() +
                                  " but get " + rhs_type->GetName());
                }
            }

            Message::set_current_part("Parser");
        }

        return std::make_unique<AST::ArithmeticExpr>(type, std::move(lhs),
                                                     std::move(rhs));
    } else {
        return lhs;
    }
    return nullptr;
}

std::unique_ptr<AST::ASTNode> Statement::ParseMultiplicativeExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto lhs = ParseUnaryExpr(context, tokens);
    if (lhs == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for multiplicative "
                                  "expression failed";
        return nullptr;
    }

    // if we have a multiplicative, we should parse the next
    // expression
    if (peek(tokens).Type() == Lexical::TokenType::kMul ||
        peek(tokens).Type() == Lexical::TokenType::kDiv ||
        peek(tokens).Type() == Lexical::TokenType::kMod) {
        auto type = peek(tokens);
        pop_list(tokens);
        auto rhs = ParseMultiplicativeExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for multiplicative "
                                      "expression failed";
            return nullptr;
        }

        if (Options::Global_enable_type_checking) {
            Message::set_current_part("Type checking");

            // LHS and rhs should not be void
            if (lhs->GetType()->GetName(true) == "void" ||
                rhs->GetType()->GetName() == "void") {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation void " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " void")
            }

            // lhs and rhs should not be arrayed
            if (lhs->GetType()->IsArray() || rhs->GetType()->IsArray()) {
                MYCC_PrintTokenError_ReturnNull(
                    type,
                    "No match for binary operation " +
                        lhs->GetType()->GetName() + " " +
                        Lexical::SymbolUtils::TokenTypeToString(type.Type()) +
                        " " + rhs->GetType()->GetName())
            }

            // LHS == RHS
            if (!lhs->GetType()->IsSame(rhs->GetType())) {
                auto rhs_type = rhs->GetType();
                rhs = AST::ASTNode::CastTo(lhs->GetType(), std::move(rhs));
                if (rhs == nullptr) {
                    MYCC_PrintTokenError_ReturnNull(
                        type, "Expect type " + lhs->GetType()->GetName() +
                                  " but get " + rhs_type->GetName());
                }
            }

            Message::set_current_part("Parser");
        }

        return std::make_unique<AST::ArithmeticExpr>(type, std::move(lhs),
                                                     std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseUnaryExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    // some of unary expression does not have LHS
    if (peek(tokens).Type() == Lexical::TokenType::kSub ||
        peek(tokens).Type() == Lexical::TokenType::kLogicalNot ||
        peek(tokens).Type() == Lexical::TokenType::kReference ||
        peek(tokens).Type() == Lexical::TokenType::kBitWiseNot ||
        peek(tokens).Type() == Lexical::TokenType::kDereference) {
        auto type = peek(tokens);
        pop_list(tokens);
        auto rhs = ParseUnaryExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for unary expression failed";
            return nullptr;
        }

        return std::make_unique<AST::UnaryExpr>(type, std::move(rhs));
    }

    // for cast expression, we should parse the next expression
    else if (peek(tokens).Type() == Lexical::TokenType::kLParentheses &&
             tokens.size() >= 2 && context.hasType(peek2(tokens).Value())) {
        auto cast_loc = peek(tokens);
        pop_list(tokens);  // consume the '('

        // get the type of the cast expression
        auto type_name = ParseTypeName(context, tokens);

        if (type_name.empty()) {
            MYCC_PrintTokenError(peek(tokens), "Expected type name");
            return nullptr;
        }

        auto [argument_type, attrs, name_token] =
            ParseTypeDecl(TokenListToString(type_name), context, tokens);

        // consume the ')'
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses,
                                        tokens);

        auto rhs = ParseUnaryExpr(context, tokens);
        if (rhs == nullptr) {
            VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for cast expression failed";
            return nullptr;
        }

        if (Options::Global_enable_type_checking) {
            Message::set_current_part("Type checking");
            // Type == RHS

            if ((argument_type->IsArray() && !rhs->GetType()->IsArray()) ||
                (!argument_type->IsArray() && rhs->GetType()->IsArray()) ||
                (argument_type->GetName() == "void" ||
                 rhs->GetType()->GetName() == "void")) {
                MYCC_PrintTokenError_ReturnNull(
                    cast_loc, "Cannot cast expression of type " +
                                  rhs->GetType()->GetName() + " to " +
                                  argument_type->GetName());
            }

            Message::set_current_part("Parser");
        }

        if (rhs->GetType()->IsConst()) {
            return std::make_unique<AST::CastExpr>(
                argument_type->GetConstType(), std::move(rhs),
                cast_loc.Location());
        } else {
            return std::make_unique<AST::CastExpr>(
                argument_type, std::move(rhs), cast_loc.Location());
        }
    }

    // sizeof expr
    else if (peek(tokens).Type() == Lexical::TokenType::kSizeOf) {
        auto size_of_loc = peek(tokens).Location();
        pop_list(tokens);  // consume the sizeof

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
        if (peek(tokens).Type() == Lexical::TokenType::kSelfIncrement ||
            peek(tokens).Type() == Lexical::TokenType::kSelfDecrement) {
            auto type = peek(tokens);
            pop_list(tokens);

            // parsing RHS
            auto rhs = ParseUnaryExpr(context, tokens);
            if (rhs == nullptr) {
                VLOG(SYNTAX_LOG_LEVEL) << "Parse [RHS] for unary "
                                          "expression failed";
                return nullptr;
            }

            // LHS has to be a variable Node
            if (!rhs->IsAssignable()) {
                MYCC_PrintTokenError_ReturnNull(type,
                                                "cannot add value to rvalue");
            }

            // LHS has to be a variable Node
            if (rhs->GetType()->IsConst()) {
                MYCC_PrintTokenError_ReturnNull(
                    type, "cannot set value to const variable");
            }

            return std::make_unique<AST::UnaryExpr>(type, std::move(rhs));
        } else {
            auto lhs = ParseAccessExpr(context, tokens);
            if (lhs == nullptr) {
                VLOG(SYNTAX_LOG_LEVEL) << "Parse [LHS] for unary "
                                          "expression failed";
                return nullptr;
            }

            // check the next token
            if (peek(tokens).Type() == Lexical::TokenType::kSelfIncrement) {
                // LHS has to be a variable Node
                if (!lhs->IsAssignable()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "Expression is not assignable")
                }

                // LHS has to be a variable Node
                if (lhs->GetType()->IsConst()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "cannot set value to const variable")
                }

                return std::make_unique<AST::UnaryExpr>(pop_list(tokens),
                                                        std::move(lhs));
            } else if (peek(tokens).Type() ==
                       Lexical::TokenType::kSelfDecrement) {
                // LHS has to be a variable Node
                if (!lhs->IsAssignable()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "Expression is not assignable")
                }

                // LHS has to be a variable Node
                if (lhs->GetType()->IsConst()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "cannot set value to const variable");
                }

                return std::make_unique<AST::UnaryExpr>(pop_list(tokens),
                                                        std::move(lhs));
            } else {
                return std::move(lhs);
            }
        }
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseAccessExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    auto currentExpr = ParsePostfixExpr(context, tokens);

    if (currentExpr == nullptr) {
        VLOG(SYNTAX_LOG_LEVEL) << "Parse [EXPR] for access expression failed";
        return nullptr;
    }

    // variable accessor
    if (peek(tokens).Type() == Lexical::TokenType::kDot ||
        peek(tokens).Type() == Lexical::TokenType::kArrow) {
        do {
            if (peek(tokens).Type() == Lexical::TokenType::kArrow ||
                peek(tokens).Type() == Lexical::TokenType::kDot) {
                // we need to know is it a pointer or not
                bool isPtr =
                    pop_list(tokens).Type() == Lexical::TokenType::kArrow;

                // accessor should not be a keyword
                if (peek(tokens).Type() != Lexical::TokenType::kIdentity) {
                    MYCC_PrintFirstTokenError_ReturnNull(tokens,
                                                         "Expect identifier")
                }

                // to new a variable node
                auto memberName = pop_list(tokens);

                //  check if the type has this member
                if (Options::Global_enable_type_checking) {
                    Message::set_current_part("Type checking");

                    if (!currentExpr->GetType()->IsStruct()) {
                        MYCC_PrintFirstTokenError_ReturnNull(
                            tokens,
                            "Expect struct type for member "
                            "access")
                    }

                    auto type = dynamic_cast<AST::StructType*>(
                                    currentExpr->GetType().get())
                                    ->GetChild(memberName.Value());

                    // check member name
                    if (type == nullptr) {
                        MYCC_PrintFirstTokenError_ReturnNull(
                            tokens, "Unknown member '" + memberName.Value() +
                                        "' in " +
                                        currentExpr->GetType()->GetName())
                    }
                    Message::set_current_part("Parser");
                }

                currentExpr = std::make_unique<AST::AccessExpr>(
                    isPtr, memberName, std::move(currentExpr));
            } else {
                // consume the ']'
                auto start = tokens.front();
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
                if (Options::Global_enable_type_checking &&
                    !currentExpr->GetType()->IsArray()) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens, "Expect array type for array accessor")
                }

                // consume the ']'

                MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRBracket,
                                                tokens);
                currentExpr = std::make_unique<AST::ArraySubscriptExpr>(
                    start, std::move(currentExpr), std::move(indexExpr));
            }
        } while (peek(tokens).Type() == Lexical::TokenType::kDot ||
                 peek(tokens).Type() == Lexical::TokenType::kArrow ||
                 peek(tokens).Type() == Lexical::TokenType::kLBracket);

        return currentExpr;
    } else {
        return currentExpr;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParsePostfixExpr(
    AST::CompilationUnit& context, TokenList& tokens) {
    // first handle priority expression ("()")
    if (peek(tokens).Type() == Lexical::TokenType::kLParentheses) {
        pop_list(tokens);  // consume the '('

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
    else if (peek(tokens).Type() == Lexical::TokenType::kIdentity &&
             !context.hasType(peek(tokens).Value())) {
        auto name = pop_list(tokens);

        // array
        if (peek(tokens).Type() == Lexical::TokenType::kLBracket) {
            auto start = pop_list(tokens);  // consume the '['

            // array accessor should be integer
            auto index = ParseCommaExpr(context, tokens);
            if (index == nullptr) {
                VLOG(SYNTAX_LOG_LEVEL) << "Parse [INDEX] for array "
                                          "accessor failed";
                return nullptr;
            }

            // should be integer
            if (Options::Global_enable_type_checking) {
                Message::set_current_part("Type checking");
                auto int_type = AST::Type::GetBasicType("int", {});
                if (!int_type->IsSame(index->GetType())) {
                    auto index_type = index->GetType();
                    index = AST::ASTNode::CastTo(int_type, std::move(index));
                    if (index == nullptr) {
                        MYCC_PrintTokenError_ReturnNull(
                            start,
                            "Expect type int but get " + index_type->GetName());
                    }
                }
                Message::set_current_part("Parser");
            }

            // consume the ']'
            MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRBracket,
                                            tokens);

            if (!context.hasVariable(name.Value(), false)) {
                MYCC_PrintTokenError_ReturnNull(
                    name, "Undefined variable: " + name.Value());
            }

            auto type = context.getVariableType(name.Value());

            if (type->IsArray() || !Options::Global_enable_type_checking) {
                return std::make_unique<AST::ArraySubscriptExpr>(
                    start, std::make_unique<AST::DeclRefExpr>(name, type),
                    std::move(index));
            } else {
                MYCC_PrintTokenError_ReturnNull(
                    name, "Expect array type for array accessor");
            }
        }

        // function caller
        else if (peek(tokens).Type() == Lexical::TokenType::kLParentheses) {
            pop_list(tokens);  // consume the '('

            // check function name is matched
            if (Options::Global_enable_type_checking &&
                !context.hasFunction(name.Value())) {
                MYCC_PrintTokenError_ReturnNull(
                    name, "Function " + name.Value() + " not found");
            }

            // next token should not be ", "
            if (peek(tokens).Type() == Lexical::TokenType::kComma) {
                MYCC_PrintFirstTokenError_ReturnNull(tokens, "term expected");
            }

            // parse arguments
            std::list<std::unique_ptr<AST::ASTNode>> args_expr;
            while (peek(tokens).Type() != Lexical::TokenType::kRParentheses) {
                auto arg_token = tokens.front();
                auto arg = ParseAssignExpr(context, tokens);
                if (arg == nullptr) {
                    VLOG(SYNTAX_LOG_LEVEL) << "Parse [ARG] for function "
                                              "caller failed";
                    return nullptr;
                }

                // consume the ',' if have other arguments
                if (peek(tokens).Type() != Lexical::TokenType::kRParentheses) {
                    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kComma,
                                                    tokens);

                    // after consume the ',', we should have a
                    // valid argument
                    if (peek(tokens).Type() ==
                        Lexical::TokenType::kRParentheses) {
                        MYCC_PrintTokenError_ReturnNull(peek(tokens),
                                                        "term expected");
                    }
                }
                args_expr.push_back(std::move(arg));
            }

            // get function return type and argument types
            auto [return_type, funcType, line_no] =
                context.getFuncRetAndArgType(name.Value());

            // function call need to have same number of arguments
            if (Options::Global_enable_type_checking) {
                Message::set_current_part("Type checking");

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
                        name,
                        "Parameter mismatch in call to " +
                            return_type->GetName() + " " + name.Value() +
                            type_str + "\n\tExpected " +
                            std::to_string(funcType.size()) + ", received " +
                            std::to_string(args_expr.size()) + " parameters");
                }

                // check argument types for all exprs
                for (auto& expr : args_expr) {
                    if (!expr->GetType()->IsSame(funcType.front())) {
                        auto arg_type = expr->GetType();
                        expr = AST::ASTNode::CastTo(funcType.front(),
                                                    std::move(expr));
                        if (expr == nullptr) {
                            MYCC_PrintTokenError_ReturnNull(
                                name, "Expect type " +
                                          funcType.front()->GetName() +
                                          " for argument " +
                                          std::to_string(args_expr.size() + 1) +
                                          " but get " + arg_type->GetName());
                        }
                    }
                    funcType.pop_front();
                }

                Message::set_current_part("Parser");
            } else {
                return_type = context.getNamedType("void", {});
            }

            // consume the ')'
            MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses,
                                            tokens);

            return std::make_unique<AST::FunctionCall>(name, return_type,
                                                       std::move(args_expr));
        }

        if (context.hasVariable(name.Value(), false) ||
            !Options::Global_enable_type_checking) {
            return std::make_unique<AST::DeclRefExpr>(
                name, context.getVariableType(name.Value()));
        } else {
            MYCC_PrintTokenError_ReturnNull(
                name, "Undefined variable: " + name.Value());
        }
    }

    // const values (string literal will handle differently
    // since it could be concat)
    else if (peek(tokens).Type() == Lexical::TokenType::kChar ||
             peek(tokens).Type() == Lexical::TokenType::kInteger ||
             peek(tokens).Type() == Lexical::TokenType::kReal_number) {
        auto value = peek(tokens);

        switch (pop_list(tokens).Type()) {
            case Lexical::TokenType::kChar:
                return std::make_unique<AST::LiteralExpr>(
                    AST::LiteralExpr::kChar, value);
            case Lexical::TokenType::kInteger:
                return std::make_unique<AST::LiteralExpr>(
                    AST::LiteralExpr::kInteger, value);
                ;
            case Lexical::TokenType::kReal_number:
                return std::make_unique<AST::LiteralExpr>(
                    AST::LiteralExpr::kReal_number, value);
            default:
                LOG(FATAL) << "Unreachable code: \n"
                           << "\t Current AST:\n";
                return nullptr;
        }
    }

    // handle string literal
    else if (peek(tokens).Type() == Lexical::TokenType::kString) {
        auto value = pop_list(tokens);
        auto value_str = value.Value();

        while (peek(tokens).Type() == Lexical::TokenType::kString) {
            value_str += pop_list(tokens).Value();
        }

        return std::make_unique<AST::LiteralExpr>(
            AST::LiteralExpr::kString,
            Lexical::Token(value_str, value.Type(), value.Location().first,
                           value.Location().second));
    }

    // if we can detect types
    else {
        if (peek(tokens).Type() == Lexical::TokenType::kElse) {
            MYCC_PrintFirstTokenError_ReturnNull(
                tokens, "Else has to be followed by if");
        } else {
            MYCC_PrintFirstTokenError_ReturnNull(tokens, "term expected")
        }
    }
}

}  // namespace Mycc::Syntax::Parser
