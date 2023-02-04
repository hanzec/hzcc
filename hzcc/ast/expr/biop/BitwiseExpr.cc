//
// Created by Hanze Chen on 2022/3/29.
//
#include <glog/logging.h>

#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "ast/Stmt.h"
#include "ast/expr/Expr.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

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
        LOG_IF(FATAL, type.length() != 0)
            << UniqueName() << "type string empty";
        LOG_IF(FATAL,
               type.length() == 1 ||
                   (type.length() == 2 && (type[0] == '<' || type[0] == '>')))
            << UniqueName() << "type len mismatch";
    })

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '&':
            this->_type = BitwiseType::AND;
            break;
        case '|':
            this->_type = BitwiseType::OR;
            break;
        case '^':
            this->_type = BitwiseType::XOR;
            break;
        case '<':
            this->_type = BitwiseType::LSHIFT;
            break;
        case '>':
            this->_type = BitwiseType::RSHIFT;
            break;
        default:
            LOG(FATAL) << UniqueName()
                       << "type: [" + std::string(type) + "] not supported";
    }
}
}  // namespace hzcc::ast
