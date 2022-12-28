//
// Created by chen_ on 2022/6/15.
//

#include "ast/type/Type.h"
#include "CastExpr.h"
namespace hzcc::ast {
ArrayToPointerDecayCast::ArrayToPointerDecayCast(const Position& location,
                                                 std::unique_ptr<Expr> expr)
    : ImplicitCastExpr("ArrayToPointerDecay", location, std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, GetCastExpr()->retType()->is_ptr())
        << UniqueName() << "has to be a pointer type";

    INTERNAL_LOG_IF(FATAL, strcmp(GetCastExpr()->NodeName().data(),
                                  "ArraySubscriptExpr") == 0)
        << UniqueName() << "cast has to be a ArraySubscriptExpr";
#endif
}
std::shared_ptr<Type> ArrayToPointerDecayCast::retType() const {
    return std::dynamic_pointer_cast<ArrayType>(GetCastExpr()->retType())
        ->GetBaseType();
}
}  // namespace hzcc::ast