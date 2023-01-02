//
// Created by chen_ on 2022/6/15.
//

#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "ast/expr/Expr.h"
#include "ast/Stmt.h"
#include "ast/type/Type.h"
#include "macro.h"
#include "utils/logging.h"

namespace hzcc::ast {

IntegralCast::IntegralCast(const Position& location,    // NOLINT
                           std::shared_ptr<Type> type,  // NOLINT
                           std::unique_ptr<Expr> expr)  // NOLINT
    : ImplicitCastExpr("IntegralCast", location, std::move(expr)),
      _type(std::move(type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _type != nullptr)
        << UniqueName() << "cast type is nullptr";
    INTERNAL_LOG_IF(WARNING, (*cast_expr()->retType()) != *_type)
        << UniqueName() << "cast type is not equal to cast cast type";
    INTERNAL_LOG_IF(WARNING, (_type->IsNumericalType() ||
                              cast_expr()->retType()->IsNumericalType()))
        << UniqueName() << "cast type or expression type is not builtin type";
#endif
}

std::shared_ptr<Type> IntegralCast::retType() const { return _type; }
}  // namespace hzcc::ast