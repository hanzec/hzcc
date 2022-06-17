//
// Created by chen_ on 2022/3/29.
//
#include "CastExpr.h"

#include <algorithm>

#include "AST/type/Type.h"
#include "AST/utils/macro.h"

namespace Hzcc::AST {
CastExpr::CastExpr(const Position& location,       // NOLINT
                   std::unique_ptr<ASTNode> expr)  // NOLINT
    : ASTNode(location),
      _cast_expr(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_cast_expr != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("cast expr is nullptr", this);
}

bool CastExpr::IsReturnLValue() const { return _cast_expr->IsReturnLValue(); }
Status CastExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
const std::unique_ptr<ASTNode>& CastExpr::GetCastExpr() const { return _cast_expr; }
bool CastExpr::IsLiteral() const { return _cast_expr->IsLiteral(); }
const char* CastExpr::NodeName() const { return "AbstractCastExpr"; }
}  // namespace Hzcc::AST
