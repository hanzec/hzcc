//
// Created by Hanze Chen on 2022/3/29.
//
#include <cstring>

#include "ast/expr/Expr.h"

namespace hzcc::ast {
BitwiseExpr::BitwiseExpr(const Position& loc,               // NOLINT
                         std::string_view type,             // NOLINT
                         std::unique_ptr<Expr> left_expr,   // NOLINT
                         std::unique_ptr<Expr> right_expr)  // NOLINT
    : OperatorBase(loc, "BitwiseExpr", std::move(right_expr),
                   std::move(left_expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK_BLOCK({
        INTERNAL_LOG_IF(FATAL, type.length() != 0)
            << UniqueName() << "type string empty";
        INTERNAL_LOG_IF(
            FATAL, type.length() == 1 || (type.length() == 2 &&
                                          (type[0] == '<' || type[0] == '>')))
            << UniqueName() << "type len mismatch";
    })

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '&':
            this->_type = BitwiseType::kBitwiseType_AND;
            break;
        case '|':
            this->_type = BitwiseType::kBitwiseType_OR;
            break;
        case '^':
            this->_type = BitwiseType::kBitwiseType_XOR;
            break;
        case '<':
            this->_type = BitwiseType::kBitwiseType_LSHIFT;
            break;
        case '>':
            this->_type = BitwiseType::kBitwiseType_RSHIFT;
            break;
        default:
            INTERNAL_LOG(FATAL)
                << UniqueName()
                << "type: [" + std::string(type) + "] not supported";
    }
}
Status BitwiseExpr::visit(Visitor& visitor) { return visitor.visit(this); }
}  // namespace hzcc::ast
