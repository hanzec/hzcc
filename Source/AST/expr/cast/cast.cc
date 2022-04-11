//
// Created by chen_ on 2022/3/29.
//
#include "cast.h"

#include <algorithm>

namespace Mycc::AST {

std::string AST::CastExpr::GetNodeName() const { return "CastExpr"; }
bool CastExpr::IsDeducible() const { return _cast_expr->IsDeclNode(); }
bool CastExpr::IsAssignable() const { return _cast_expr->IsAssignable(); }
std::shared_ptr<Type> CastExpr::GetType() const { return _cast_type; }

std::string CastExpr::PrintAdditionalInfo(std::string_view ident) const {
    auto new_ident = std::string(ident);
    std::replace(new_ident.begin(), new_ident.end(), '`', ' ');
    return "\n" + _cast_expr->Dump(new_ident + " `");
}

void CastExpr::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}

}  // namespace Mycc::AST
