//
// Created by Hanze Chen on 2022/3/29.
//
#include "OperatorBase.h"

#include <cstring>

namespace hzcc::ast {
BitwiseExpr::BitwiseExpr(const char* type, const Position& loc,
                         std::unique_ptr<Expr> left_expr,
                         std::unique_ptr<Expr> right_expr)  // NOLINT
    : OperatorBase(loc, "BitwiseExpr", std::move(right_expr),
                   std::move(left_expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, type != nullptr || strlen(type) != 0)
        << UniqueName() << "type string empty";
    INTERNAL_LOG_IF(
        FATAL, strlen(type) == 1 ||
                   (strlen(type) == 2 && (type[0] == '<' || type[0] == '>')))
        << UniqueName() << "type len mismatch";
#endif

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
