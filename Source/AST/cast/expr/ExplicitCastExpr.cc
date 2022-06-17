//
// Created by chen_ on 2022/6/13.
//

#include "ExplicitCastExpr.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
ExplicitCastExpr::ExplicitCastExpr(const Position& location,
                               std::shared_ptr<Type> to_type,
                               std::unique_ptr<ASTNode> expr)
    : CastExpr(location, std::move(expr)), _cast_type(std::move(to_type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_cast_type != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("cast type is nullptr", this);
    HZCC_RUNTIME_WARNING((*GetCastExpr()->RetType()) != *_cast_type)
        << HZCC_AST_PRINT_NODE_INFO("cast type is equal to expr type",
                                           this);
}

const char* ExplicitCastExpr::NodeName() const { return "ExplicitCastExpr"; }

void ExplicitCastExpr::PrintDetail(std::ostream& out,
                                 const std::string& ident) const {
    out << '[' << GetCastExpr()->RetType()->Name() << "]->["
        << RetType()->Name() << "]";
    GetCastExpr()->Dump(out, ident + " `");
}

}  // namespace Hzcc::AST