//
// Created by chen_ on 2022/3/29.
//
#include "cast.h"

#include <algorithm>

namespace Hzcc::AST {

const char* AST::CastExpr::NodeName() const { return "CastExpr"; }
bool CastExpr::IsAssignable() const { return _cast_expr->IsAssignable(); }
std::shared_ptr<Type> CastExpr::GetType() const { return _cast_type; }

std::string CastExpr::PrintAdditionalInfo(const std::string& ident) const {
    return "[" + _cast_expr->GetType()->GetName() + "]->[" +
           _cast_type->GetName() + "]\n" + _cast_expr->Dump(ident + " `");
}

Status CastExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::unique_ptr<ASTNode>& CastExpr::GetCastExpr() { return _cast_expr; }

}  // namespace Hzcc::AST
