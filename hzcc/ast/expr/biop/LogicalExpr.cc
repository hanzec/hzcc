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
#include "ast/type/Type.h"
#include "ast/visitor.h"
#include "enums.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc::ast {
LogicalExpr::LogicalExpr(const Position& loc,        // NOLINT
                         std::string_view type,      // NOLINT
                         std::unique_ptr<Expr> lhs,  // NOLINT
                         std::unique_ptr<Expr> rhs)  // NOLINT
    : OperatorBase(loc, "LogicalExpr", std::move(lhs), std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, !type.empty())
        << UniqueName() << "type string empty";
    INTERNAL_LOG_IF(FATAL, type.size() == 2)
        << UniqueName() << "type len mismatch";
#endif

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    if (type[0] == '&' && type[1] == '&') {
        this->_type = LogicalType::AND;
    } else if (type[0] == '|' && type[1] == '|') {
        this->_type = LogicalType::OR;
    } else {
        INTERNAL_LOG(FATAL)
            << UniqueName()
            << "type: [" + std::string(type) + "] not supported";
    }
}
}  // namespace hzcc::ast
