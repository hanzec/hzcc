//
// Created by chen_ on 2022/6/15.
//
#include "LvalueToRvalueCast.h"

#include "AST/type/PointerType.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
LvalueToRvalueCast::LvalueToRvalueCast(const Position& location,
                                       std::unique_ptr<ASTNode> expr)
    : ImplicitCastExpr("LvalueToRvalue", location, std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(GetCastExpr()->IsReturnLValue())
        << HZCC_AST_PRINT_NODE_INFO("has to return left value", this);
}
bool LvalueToRvalueCast::IsReturnLValue() const { return false; }
std::shared_ptr<Type> LvalueToRvalueCast::RetType() const {
    return GetCastExpr()->RetType();
}
}  // namespace Hzcc::AST