//
// Created by chen_ on 2022/3/29.
//
#include "Expr.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
SizeofExpr::SizeofExpr(std::unique_ptr<Stmt> expr, const Position& location)
    : Expr(location, "SizeofExpr"), _expr(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _expr != nullptr)
        << UniqueName() << "cast is nullptr";
#endif
}

std::shared_ptr<Type> SizeofExpr::retType() const {
    return GetNumericalTypeOf<PrimitiveType::kLong>();
}
Status SizeofExpr::visit(Visitor& visitor) { return visitor.visit(this); }

}  // namespace hzcc::ast
