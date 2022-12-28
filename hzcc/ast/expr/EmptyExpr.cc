//
// Created by chen_ on 2022/12/24.
//
#include "Expr.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
std::shared_ptr<Type> EmptyExpr::retType() const {
    return GetNumericalTypeOf<PrimitiveType::kVoid>();
}
Status EmptyExpr::visit(Visitor& visitor) { return NoError(); }
}  // namespace hzcc::ast