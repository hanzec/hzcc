//
// Created by chen_ on 2022/6/13.
//

#include "CastExpr.h"

namespace hzcc::ast {
ExplicitCastExpr::ExplicitCastExpr(const Position& location,    // NOLINT
                                   std::shared_ptr<Type> type,  // NOLINT
                                   std::unique_ptr<Expr> expr)  // NOLINT
    : CastExpr("ExplicitCastExpr", location, std::move(expr)),
      _cast_type(std::move(type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _cast_type != nullptr)
        << UniqueName() << "cast type is nullptr";
    INTERNAL_LOG_IF(WARNING, (*GetCastExpr()->retType()) != *_cast_type)
        << UniqueName() << "cast type is not equal to cast cast type";
#endif
}

void ExplicitCastExpr::PrintDetail(std::ostream& out,
                                   const std::string& ident) const {
    out << '[' << GetCastExpr()->retType()->Name() << "]->["
        << retType()->Name() << "]";
    GetCastExpr()->Dump(out, ident + " `");
}

}  // namespace hzcc::ast