//
// Created by chen_ on 2022/6/15.
//

#include "IntegralCast.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {

IntegralCast::IntegralCast(const Position& location,       // NOLINT
                           std::shared_ptr<Type> type,     // NOLINT
                           std::unique_ptr<ASTNode> expr)  // NOLINT
    : ImplicitCastExpr("IntegralCast", location, std::move(expr)),
      _type(std::move(type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_type != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("cast type is nullptr", this);
    HZCC_RUNTIME_WARNING((*GetCastExpr()->RetType()) != *_type)
        << HZCC_AST_PRINT_NODE_INFO("cast type is equal to expr type",
                                           this);
    HZCC_RUNTIME_WARNING(_type->IsBuiltinType() ||
                         GetCastExpr()->RetType()->IsBuiltinType())
        << HZCC_AST_PRINT_NODE_INFO(
               "cast type or expression type is not builtin type", this);
}
std::shared_ptr<Type> IntegralCast::RetType() const { return _type; }
}  // namespace Hzcc::AST