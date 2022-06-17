//
// Created by chen_ on 2022/6/15.
//

#include "ArrayToPointerDecayCast.h"

#include "AST/type/ArrayType.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
ArrayToPointerDecayCast::ArrayToPointerDecayCast(const Position& location,
                                                 std::unique_ptr<ASTNode> expr)
    : ImplicitCastExpr("ArrayToPointerDecay", location, std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(GetCastExpr()->RetType()->IsPointer())
        << HZCC_AST_PRINT_NODE_INFO("has to be a pointer type", this);

    HZCC_RUNTIME_CHECK(
        strcmp(GetCastExpr()->NodeName(), "ArraySubscriptExpr") == 0)
        << HZCC_AST_PRINT_NODE_INFO(
               "expr has to be a ArraySubscriptExpr", this);
}
std::shared_ptr<Type> ArrayToPointerDecayCast::RetType() const {
    return std::dynamic_pointer_cast<ArrayType>(GetCastExpr()->RetType())
        ->GetBaseType();
}
}  // namespace Hzcc::AST