//
// Created by chen_ on 2022/3/29.
//
#include "CastExpr.h"

#include <algorithm>

#include "ast/type/Type.h"
#include "ast/DeduceValue.h"
namespace hzcc::ast {
CastExpr::CastExpr(const char* node_name,       // NOLINT
                   const Position& location,    // NOLINT
                   std::unique_ptr<Expr> expr)  // NOLINT
    : Expr(location, node_name), _expr(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _expr != nullptr)
        << UniqueName() << "cast cast is nullptr";
#endif
}

bool CastExpr::IsReturnLValue() const { return _expr->IsReturnLValue(); }
Status CastExpr::visit(Visitor& visitor) { return visitor.visit(this); }
const std::unique_ptr<Expr>& CastExpr::GetCastExpr() const { return _expr; }
bool CastExpr::IsLiteral() const { return _expr->IsLiteral(); }
std::optional<DeduceValue> CastExpr::GetDeducedValue() const {
    return {_expr->GetDeducedValue()};
}
}  // namespace hzcc::ast
