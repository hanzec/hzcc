//
// Created by chen_ on 2022/6/15.
//

#include <glog/logging.h>
#include <string.h>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "ast/type/Type.h"
#include "ast/expr/Expr.h"
#include "ast/Stmt.h"
#include "macro.h"
#include "utils/logging.h"

namespace hzcc::ast {
ArrayToPointerDecayCast::ArrayToPointerDecayCast(const Position& location,
                                                 std::unique_ptr<Expr> expr)
    : ImplicitCastExpr("ArrayToPointerDecay", location, std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, cast_expr()->retType()->is_ptr())
        << UniqueName() << "has to be a pointer type";

    INTERNAL_LOG_IF(FATAL, strcmp(cast_expr()->NodeName().data(),
                                  "ArraySubscriptExpr") == 0)
        << UniqueName() << "cast has to be a ArraySubscriptExpr";
#endif
}
std::shared_ptr<Type> ArrayToPointerDecayCast::retType() const {
    return std::dynamic_pointer_cast<ArrayType>(cast_expr()->retType())
        ->GetBaseType();
}
}  // namespace hzcc::ast