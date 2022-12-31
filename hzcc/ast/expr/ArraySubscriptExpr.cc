//
// Created by chen_ on 2022/3/29.
//
#include "Expr.h"

namespace hzcc::ast {
ArraySubscriptExpr::ArraySubscriptExpr(
    const Position& location,         // NOLINT
    std::unique_ptr<Expr> array,      // NOLINT
    std::unique_ptr<Expr> subscript)  // NOLINT
    : Expr(location, "ArraySubscriptExpr"),
      _array(std::move(array)),
      _subscript(std::move(subscript)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _array != nullptr)
        << UniqueName() << "array is nullptr";
    INTERNAL_LOG_IF(FATAL, _subscript != nullptr)
        << UniqueName() << "subscript is nullptr";
    INTERNAL_LOG_IF(FATAL, _array->retType()->IsArray())
        << UniqueName() << "not accessing array";
#endif
}

std::shared_ptr<Type> ArraySubscriptExpr::retType() const {
    return std::dynamic_pointer_cast<ast::ArrayType>(_array->retType())
        ->GetBaseType();
}

bool ArraySubscriptExpr::IsReturnLValue() const { return true; }

Status ArraySubscriptExpr::visit(Visitor& visitor) {
    return visitor.visit(this);
}

bool ArraySubscriptExpr::IsDereference() const { return true; }
std::unique_ptr<Expr>& ArraySubscriptExpr::base_expr() { return _array; }
std::unique_ptr<Expr>& ArraySubscriptExpr::subscript_expr() { return _subscript; }
}  // namespace hzcc::ast
