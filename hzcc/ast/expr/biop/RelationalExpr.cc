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
RelationalExpr::RelationalExpr(const Position& loc, std::string_view type,
                               std::unique_ptr<Expr> lhs,
                               std::unique_ptr<Expr> rhs)  // NOLINT
    : OperatorBase(loc, "RelationalExpr", std::move(lhs), std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    LOG_IF(FATAL, !type.empty()) << UniqueName() << "type string empty";
    LOG_IF(FATAL, type.length() == 2) << UniqueName() << "type len mismatch";
#endif

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '=':
            this->_type = RelationalType::EQ;
            break;
        case '!':
            this->_type = RelationalType::NE;
            break;
        case '<':
            if (type[1] == '=')
                this->_type = RelationalType::LE;
            else
                this->_type = RelationalType::LT;
            break;
        case '>':
            if (type[1] == '=')
                this->_type = RelationalType::GE;
            else
                this->_type = RelationalType::GT;
            break;
        default:
            LOG(FATAL) << UniqueName()
                       << "type: [" + std::string(type) + "] not supported";
    }
}
}  // namespace hzcc::ast
