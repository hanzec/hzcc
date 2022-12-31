//
// Created by chen_ on 2022/6/15.
//
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
LvalueToRvalueCast::LvalueToRvalueCast(const Position& location,
                                       std::unique_ptr<Expr> expr)
    : ImplicitCastExpr("LvalueToRvalue", location, std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, cast_expr()->IsReturnLValue())
        << "has to return left value";
#endif
}

bool LvalueToRvalueCast::IsReturnLValue() const { return false; }
std::shared_ptr<Type> LvalueToRvalueCast::retType() const {
    return cast_expr()->retType();
}
}  // namespace hzcc::ast