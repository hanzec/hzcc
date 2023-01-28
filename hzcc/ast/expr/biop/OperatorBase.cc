//
// Created by chen_ on 2022/3/29.
//
#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <optional>
#include <string_view>

#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "ast/Stmt.h"
#include "macro.h"
#include "utils/logging.h"
#include "ast/DeduceValue.h" // IWYU pragma: keep

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
    INTERNAL_LOG_IF(FATAL, !_lhs->retType()->IsArray())
        << UniqueName() << "lhs could not be array";
    INTERNAL_LOG_IF(FATAL, !_rhs->retType()->IsArray())
        << UniqueName() << "rhs could not be array";
#endif
}

std::unique_ptr<Expr>& OperatorBase::lhs()  { return _lhs; }
std::unique_ptr<Expr>& OperatorBase::rhs()  { return _rhs; }

std::shared_ptr<Type> OperatorBase::retType() const {
    if (!_lhs->GetDeducedValue().has_value() &&
        !_rhs->GetDeducedValue().has_value()) {
        return _lhs->retType();
    } else {
        return _lhs->GetDeducedValue().has_value() ? _rhs->retType()
                                                   : _lhs->retType();
    }
}

}  // namespace hzcc::ast
