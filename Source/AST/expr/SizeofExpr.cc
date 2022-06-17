//
// Created by chen_ on 2022/3/29.
//
#include "SizeofExpr.h"

#include "AST/type/Type.h"
#include "AST/utils/macro.h"

namespace Hzcc::AST {
SizeofExpr::SizeofExpr(std::unique_ptr<ASTNode> expr, const Position& location)
    : ASTNode(location), _expr(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_expr != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("expr is nullptr", this);
}
const char* AST::SizeofExpr::NodeName() const { return "SizeofExpr"; }

std::shared_ptr<Type> SizeofExpr::RetType() const { return GetIntType(); }
Status SizeofExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }

}  // namespace Hzcc::AST
