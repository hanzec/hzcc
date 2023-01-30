//
// Created by chen_ on 2022/3/29.
//
#include <glog/logging.h>

#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "ast/DeduceValue.h"  // IWYU pragma: keep
#include "ast/Stmt.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "macro.h"
#include "utils/logging.h"

namespace hzcc::ast {
OperatorBase::OperatorBase(const Position& loc,        // NOLINT
                           const char* node_name,      // NOLINT
                           std::unique_ptr<Expr> lhs,  // NOLINT
                           std::unique_ptr<Expr> rhs)  // NOLINT
    : Expr(loc, node_name), _lhs(std::move(lhs)), _rhs(std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _lhs != nullptr) << UniqueName() << "lhs is nullptr";
    INTERNAL_LOG_IF(FATAL, _rhs != nullptr) << UniqueName() << "rhs is nullptr";
    INTERNAL_LOG_IF(FATAL, !_lhs->type()->IsArray())
        << UniqueName() << "lhs could not be array";
    INTERNAL_LOG_IF(FATAL, !_rhs->type()->IsArray())
        << UniqueName() << "rhs could not be array";
#endif
}

}  // namespace hzcc::ast
