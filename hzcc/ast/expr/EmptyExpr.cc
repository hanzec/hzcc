//
// Created by chen_ on 2022/12/24.
//
#include <memory>

#include "Expr.h"
#include "ast/type/Type.h"
#include "enums.h"
#include "utils/status/status.h"

namespace hzcc {
namespace ast {
class Visitor;
}  // namespace ast
}  // namespace hzcc

namespace hzcc::ast {
std::shared_ptr<Type> EmptyExpr::retType() const {
    return GetNumericalTypeOf<PrimitiveType::kVoid>();
}
Status EmptyExpr::visit(Visitor& visitor) { return NoError(); }
}  // namespace hzcc::ast