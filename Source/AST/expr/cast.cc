//
// Created by chen_ on 2022/3/29.
//
#include "cast.h"

namespace Mycc::AST {

std::string AST::CastExpr::GetNodeName() const { return "CastExpr"; }
bool CastExpr::IsDeducible() const { return _cast_expr->IsDeclNode(); }
bool CastExpr::IsAssignable() const { return _cast_expr->IsAssignable(); }
std::shared_ptr<Type> CastExpr::GetType() const { return _cast_type; }
}