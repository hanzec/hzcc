//
// Created by chen_ on 2022/3/29.
//
#include "CastExpr.h"

#include <algorithm>

#include "AST/type/Type.h"
#include "AST/utils/macro.h"

namespace Hzcc::AST {
CastExpr::CastExpr(const Position& location, std::shared_ptr<Type> type,
                   std::unique_ptr<ASTNode> expr)  // NOLINT
    : ASTNode(location),
      _cast_type(std::move(type)),
      _cast_expr(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_cast_type != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("cast type nullptr", this);
    HZCC_RUNTIME_CHECK(_cast_expr != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("cast expr is nullptr", this);
    HZCC_RUNTIME_CHECK((*_cast_type) != (*_cast_expr->RetType()))
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("case to same type", this);
}

const char* AST::CastExpr::NodeName() const { return "CastExpr"; }
bool CastExpr::IsReturnLValue() const { return _cast_expr->IsReturnLValue(); }
std::shared_ptr<Type> CastExpr::RetType() const { return _cast_type; }

void CastExpr::PrintDetail(std::ostream& out, const std::string& ident) const {
    out << '[' << _cast_expr->RetType()->GetName() << "]->["
        << _cast_type->GetName() << "]";
    _cast_expr->Dump(out, ident + " `");
}

Status CastExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::unique_ptr<ASTNode>& CastExpr::GetCastExpr() { return _cast_expr; }
bool CastExpr::IsLiteral() const { return _cast_expr->IsLiteral(); }

}  // namespace Hzcc::AST
