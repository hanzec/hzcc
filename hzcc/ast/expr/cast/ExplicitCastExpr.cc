//
// Created by chen_ on 2022/6/13.
//

#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "ast/expr/Expr.h"
#include "ast/Stmt.h"
#include "macro.h"
#include "utils/logging.h"

namespace hzcc::ast {
ExplicitCastExpr::ExplicitCastExpr(
    const Position& loc, std::unique_ptr<Expr> expr,
    std::unique_ptr<TypeProxyExpr> type)  // NOLINT
    : CastExpr("ExplicitCastExpr", loc, std::move(expr)),
      _cast_type(std::move(type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _cast_type != nullptr)
        << UniqueName() << "cast type is nullptr";
    INTERNAL_LOG_IF(WARNING, cast_expr()->retType() != _cast_type->retType())
        << UniqueName() << "cast type is not equal to cast cast type";
#endif
}

}  // namespace hzcc::ast