//
// Created by chen_ on 2022/3/24.
//
#include "statement_parser.h"

#include <list>

#include "AST/ast_context.h"
#include "AST/operator/access.h"
#include "AST/operator/arithmetic.h"
#include "AST/operator/array.h"
#include "AST/operator/assign.h"
#include "AST/operator/bitwise.h"
#include "AST/operator/cast.h"
#include "AST/operator/comma.h"
#include "AST/operator/logical.h"
#include "AST/operator/relational.h"
#include "AST/operator/sizeof.h"
#include "AST/operator/unary.h"
#include "AST/statement/break.h"
#include "AST/statement/continue.h"
#include "AST/statement/do_parser.h"
#include "AST/statement/for_parser.h"
#include "AST/statement/if_parser.h"
#include "AST/statement/return.h"
#include "AST/value/value.h"
#include "lexical/Token.h"
#include "lexical/token_type.h"
#include "syntax/Parser.h"
#include "syntax/parser/base_parser.h"
#include "syntax/utils/common_utils.h"

namespace Mycc::Syntax::Parser {
using namespace TokenListUtils;
Statement::Statement()
    : ParserBase(TypeNameUtil::hash<AST::ASTNode>(),
                 TypeNameUtil::name_pretty<AST::ASTNode>()) {}

std::unique_ptr<AST::ASTNode> Statement::parse_impl(  // NOLINT
    AST::ASTContext& context,                         // NOLINT
    std::list<Lexical::Token>& tokens,                // NOLINT
    const std::list<Lexical::Token>& attributes) {    // NOLINT

    std::unique_ptr<AST::ASTNode> node{nullptr};
    switch (peek(tokens).Type()) {
        case Lexical::TokenType::kReturn:
            tokens.pop_front();
            node = std::make_unique<AST::ReturnNode>(
                ParseCommaExpr(context, tokens));
            break;
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
            tokens.pop_front();
            node = std::make_unique<AST::BreakStatement>();
            break;
        case Lexical::TokenType::kContinue:
            tokens.pop_front();
            node = std::make_unique<AST::ContinueStatement>();
            break;
        default:
            /*
             * Parsing operator here
             */
            node = ParseCommaExpr(context, tokens);
            break;
    }

    // we need to consume the ;
    MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kSemiColon, tokens);
    return node;
}

std::unique_ptr<AST::ASTNode> Statement::ParseCommaExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    // parsing left-hand side of expression
    auto lhs = ParseAssignExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for comma expression failed";
        return nullptr;
    }

    // if we have a comma, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kComma) {
        tokens.pop_front();
        auto rhs = ParseCommaExpr(context, tokens);
        if (lhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for comma expression failed";
            return nullptr;
        }

        return std::make_unique<AST::CommaExpr>(std::move(lhs), std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseAssignExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseConditionalExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for assign expression failed";
        return nullptr;
    }

    // if we have a assign operator, we should parse the right-hand side
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
        auto assign_type = tokens.front().Value();
        tokens.pop_front();

        // parse rhs expression
        auto rhs = ParseAssignExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for assign expression failed";
            return nullptr;
        }

        return std::make_unique<AST::AssignExpr>(assign_type, std::move(lhs),
                                                 std::move(rhs));
    } else {
        return lhs;
    }
}

// TODO: add parser for conditional expression
std::unique_ptr<AST::ASTNode> Statement::ParseConditionalExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    return ParseLogicalOrExpr(context, tokens);
    ;
}

std::unique_ptr<AST::ASTNode> Statement::ParseLogicalOrExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseLogicalAndExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for logical or expression failed";
        return nullptr;
    }

    // if we have a logical or, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kLogicalOr) {
        tokens.pop_front();
        auto rhs = ParseLogicalOrExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for logical or expression failed";
            return nullptr;
        }

        return std::make_unique<AST::LogicalExpr>("||", std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseLogicalAndExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseBitwiseOrExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for logical and expression failed";
        return nullptr;
    }

    // if we have a logical and, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kLogicalAnd) {
        tokens.pop_front();
        auto rhs = ParseLogicalAndExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for logical and expression failed";
            return nullptr;
        }

        return std::make_unique<AST::LogicalExpr>("&&", std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseBitwiseOrExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseBitwiseXorExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for bitwise or expression failed";
        return nullptr;
    }

    // if we have a bitwise or, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kBitwiseOr) {
        tokens.pop_front();
        auto rhs = ParseBitwiseOrExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for bitwise or expression failed";
            return nullptr;
        }

        return std::make_unique<AST::BitwiseExpr>("|", std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseBitwiseXorExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseBitwiseAndExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for bitwise xor expression failed";
        return nullptr;
    }

    // if we have a bitwise xor, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kBitwiseXor) {
        tokens.pop_front();
        auto rhs = ParseBitwiseXorExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for bitwise xor expression failed";
            return nullptr;
        }

        return std::make_unique<AST::BitwiseExpr>("^", std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseBitwiseAndExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseEqualityExpr(context, tokens);

    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for bitwise and expression failed";
        return nullptr;
    }

    // if we have a bitwise and, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kBitwiseAnd) {
        tokens.pop_front();
        auto rhs = ParseBitwiseAndExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for bitwise and expression failed";
            return nullptr;
        }

        return std::make_unique<AST::BitwiseExpr>("&", std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseEqualityExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseRelationalExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for equality expression failed";
        return nullptr;
    }

    // if we have a equality, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kEqual ||
        peek(tokens).Type() == Lexical::TokenType::kNotEqual) {
        auto type = peek(tokens).Value();
        tokens.pop_front();
        auto rhs = ParseEqualityExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for equality expression failed";
            return nullptr;
        }

        return std::make_unique<AST::RelationalExpr>(type, std::move(lhs),
                                                     std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseRelationalExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseShiftExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for relational expression failed";
        return nullptr;
    }

    // if we have a relational, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kLess ||
        peek(tokens).Type() == Lexical::TokenType::kGreater ||
        peek(tokens).Type() == Lexical::TokenType::kLessEqual ||
        peek(tokens).Type() == Lexical::TokenType::kGreaterEqual) {
        auto type = peek(tokens).Value();
        tokens.pop_front();
        auto rhs = ParseRelationalExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for relational expression failed";
            return nullptr;
        }

        return std::make_unique<AST::RelationalExpr>(type, std::move(lhs),
                                                     std::move(rhs));
    } else {
        return lhs;
    }
    return nullptr;
}

std::unique_ptr<AST::ASTNode> Statement::ParseShiftExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseAdditiveExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for shift expression failed";
        return nullptr;
    }

    // if we have a shift, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kLeftShift ||
        peek(tokens).Type() == Lexical::TokenType::kRightShift) {
        auto type = peek(tokens).Value();
        tokens.pop_front();
        auto rhs = ParseShiftExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for shift expression failed";
            return nullptr;
        }

        return std::make_unique<AST::BitwiseExpr>(type, std::move(lhs),
                                                  std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseAdditiveExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseMultiplicativeExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for additive expression failed";
        return nullptr;
    }

    // if we have a additive, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kAdd ||
        peek(tokens).Type() == Lexical::TokenType::kSub) {
        auto type = peek(tokens).Value();
        tokens.pop_front();
        auto rhs = ParseAdditiveExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for additive expression failed";
            return nullptr;
        }

        return std::make_unique<AST::ArithmeticExpr>(type, std::move(lhs),
                                                     std::move(rhs));
    } else {
        return lhs;
    }
    return nullptr;
}

std::unique_ptr<AST::ASTNode> Statement::ParseMultiplicativeExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    auto lhs = ParseUnaryExpr(context, tokens);
    if (lhs == nullptr) {
        DLOG(ERROR) << "Parse [LHS] for multiplicative expression failed";
        return nullptr;
    }

    // if we have a multiplicative, we should parse the next expression
    if (peek(tokens).Type() == Lexical::TokenType::kMul ||
        peek(tokens).Type() == Lexical::TokenType::kDiv ||
        peek(tokens).Type() == Lexical::TokenType::kMod) {
        auto type = peek(tokens).Value();
        tokens.pop_front();
        auto rhs = ParseMultiplicativeExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for multiplicative expression failed";
            return nullptr;
        }

        return std::make_unique<AST::ArithmeticExpr>(type, std::move(lhs),
                                                     std::move(rhs));
    } else {
        return lhs;
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParseUnaryExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    // some of unary expression does not have LHS
    if (peek(tokens).Type() == Lexical::TokenType::kSub ||
        peek(tokens).Type() == Lexical::TokenType::kLogicalNot ||
        peek(tokens).Type() == Lexical::TokenType::kReference ||
        peek(tokens).Type() == Lexical::TokenType::kBitWiseNot ||
        peek(tokens).Type() == Lexical::TokenType::kDereference) {
        auto type = peek(tokens).Type();
        tokens.pop_front();
        auto rhs = ParseUnaryExpr(context, tokens);
        if (rhs == nullptr) {
            DLOG(ERROR) << "Parse [RHS] for unary expression failed";
            return nullptr;
        }

        AST::UnaryExpr::UnaryType unaryType;
        if (type == Lexical::TokenType::kSub) {
            unaryType = AST::UnaryExpr::UnaryType::kUnaryMinus;
        } else if (type == Lexical::TokenType::kLogicalNot) {
            unaryType = AST::UnaryExpr::UnaryType::kLogicalNot;
        } else if (type == Lexical::TokenType::kReference) {
            unaryType = AST::UnaryExpr::UnaryType::kReference;
        } else if (type == Lexical::TokenType::kBitWiseNot) {
            unaryType = AST::UnaryExpr::UnaryType::kBitwiseNot;
        } else if (type == Lexical::TokenType::kDereference) {
            unaryType = AST::UnaryExpr::UnaryType::kDereference;
        } else {
            DLOG(ERROR) << "Unknown unary type";
            return nullptr;
        }
        return std::make_unique<AST::UnaryExpr>(unaryType, std::move(rhs));
    }

    // for cast expression, we should parse the next expression
    else if (peek(tokens).Type() == Lexical::TokenType::kLParentheses ||
             context.hasType(peek2(tokens).Value())) {
        tokens.pop_front();  // consume the '('

        // get the type of the cast expression
        auto type = context.getType(tokens.front().Value());
        tokens.pop_front();  // consume the type

        // consume the ')'
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses,
                                        tokens);

        return std::make_unique<AST::CastExpr>(type);
    }

    // sizeof operator
    else if (peek(tokens).Type() == Lexical::TokenType::kSizeOf) {
        tokens.pop_front();  // consume the sizeof

        // consume the '('
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kLParentheses,
                                        tokens);

        auto expr = ParseUnaryExpr(context, tokens);
        if (expr == nullptr) {
            DLOG(ERROR) << "Parse [EXPR] for sizeof expression failed";
            return nullptr;
        }

        // consume the ')'
        MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRParentheses,
                                        tokens);

        return std::make_unique<AST::SizeofExpr>(std::move(expr));
    }

    // selfInc/selfDec and otherwise
    else {
        // since self inc/dec could before the variable or after the
        // variable we need to check the next token
        if (peek(tokens).Type() == Lexical::TokenType::kSelfIncrement ||
            peek(tokens).Type() == Lexical::TokenType::kSelfDecrement) {
            auto type = peek(tokens).Type();
            tokens.pop_front();

            // parsing RHS
            auto rhs = ParseUnaryExpr(context, tokens);
            if (rhs == nullptr) {
                DLOG(ERROR) << "Parse [RHS] for unary expression failed";
                return nullptr;
            }

            AST::UnaryExpr::UnaryType unaryType;
            if (type == Lexical::TokenType::kSelfIncrement) {
                unaryType = AST::UnaryExpr::UnaryType::kPreDec;
            } else if (type == Lexical::TokenType::kSelfDecrement) {
                unaryType = AST::UnaryExpr::UnaryType::kPreDec;
            } else {
                DLOG(ERROR) << "Unknown unary type";
                return nullptr;
            }
            return std::make_unique<AST::UnaryExpr>(unaryType, std::move(rhs));
        } else {
            auto lhs = ParsePostfixExpr(context, tokens);
            if (lhs == nullptr) {
                DLOG(ERROR) << "Parse [LHS] for unary expression failed";
                return nullptr;
            }

            // check the next token
            if (peek(tokens).Type() == Lexical::TokenType::kSelfIncrement) {
                tokens.pop_front();
                return std::make_unique<AST::UnaryExpr>(
                    AST::UnaryExpr::UnaryType::kPostInc, std::move(lhs));
            } else if (peek(tokens).Type() ==
                       Lexical::TokenType::kSelfDecrement) {
                tokens.pop_front();
                return std::make_unique<AST::UnaryExpr>(
                    AST::UnaryExpr::UnaryType::kPostDec, std::move(lhs));
            } else {
                return std::move(lhs);
            }
        }
    }
}

std::unique_ptr<AST::ASTNode> Statement::ParsePostfixExpr(
    AST::ASTContext& context, std::list<Lexical::Token>& tokens) {
    // first handle priority expression ("()")
    if (peek(tokens).Type() == Lexical::TokenType::kLParentheses) {
        tokens.pop_front();  // consume the '('

        auto expr = ParseCommaExpr(context, tokens);
        if (expr == nullptr) {
            DLOG(ERROR) << "Parse [EXPR] for postfix expression failed";
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
        auto name = pop(tokens).Value();

        // array
        if (peek(tokens).Type() == Lexical::TokenType::kLBracket) {
            tokens.pop_front();  // consume the '['

            // array accessor should be integer
            auto index = ParseCommaExpr(context, tokens);

            // consume the ']'
            MYCC_CheckAndConsume_ReturnNull(Lexical::TokenType::kRBracket,
                                            tokens);
            return std::make_unique<AST::ArrayExpr>(name, std::move(index));
        }

        // variable accessor
        else if (peek(tokens).Type() == Lexical::TokenType::kDot ||
                 peek(tokens).Type() == Lexical::TokenType::kArrow) {
            // get base type
            std::unique_ptr<AST::ASTNode> currentExpr =
                std::make_unique<AST::VariableExpr>(name);

            do {
                // we need to know is it a pointer or not
                bool isPtr = pop(tokens).Type() == Lexical::TokenType::kArrow;

                // accessor should not be a keyword
                if (peek(tokens).Type() != Lexical::TokenType::kIdentity) {
                    MYCC_PrintFirstTokenError_ReturnNull(
                        tokens,
                        "Expect [IDENTITY] after '.' " + error_token.Value())
                }

                // insert to next level
                auto memberName = pop(tokens).Value();
                currentExpr = std::make_unique<AST::AccessExpr>(
                    isPtr, memberName, std::move(currentExpr));
            } while (peek(tokens).Type() == Lexical::TokenType::kDot ||
                     peek(tokens).Type() == Lexical::TokenType::kArrow);

            return currentExpr;
        }

        return std::make_unique<AST::VariableExpr>(name);
    }

    // const values (string literal will handle differently since it
    // could be concat)
    else if (peek(tokens).Type() == Lexical::TokenType::kChar ||
             peek(tokens).Type() == Lexical::TokenType::kInteger ||
             peek(tokens).Type() == Lexical::TokenType::kReal_number) {
        auto value = peek(tokens).Value();

        switch (pop(tokens).Type()) {
            case Lexical::TokenType::kChar:
                return std::make_unique<AST::ValueNode>(AST::ValueNode::kChar,
                                                        value);
            case Lexical::TokenType::kInteger:
                return std::make_unique<AST::ValueNode>(
                    AST::ValueNode::kInteger, value);
                ;
            case Lexical::TokenType::kReal_number:
                return std::make_unique<AST::ValueNode>(
                    AST::ValueNode::kReal_number, value);
            default:
                DLOG(ERROR) << "Unreachable code: \n"
                            << "\t Current AST:\n";
                // TODO: print AST(basic debug info
                assert(false);
        }
    }

    // handle string literal
    else if (peek(tokens).Type() == Lexical::TokenType::kString) {
        auto value = pop(tokens).Value();

        while (peek(tokens).Type() == Lexical::TokenType::kString) {
            value += pop(tokens).Value();
        }

        return std::make_unique<AST::ValueNode>(AST::ValueNode::kString, value);
    }

    // error here
    else {
        MYCC_PrintFirstTokenError_ReturnNull(
            tokens, "Expect [IDENTITY] " + error_token.Value())
    }
}

}  // namespace Mycc::Syntax::Parser