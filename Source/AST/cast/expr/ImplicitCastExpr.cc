//
// Created by chen_ on 2022/6/13.
//

#include "ImplicitCastExpr.h"

#include <utility>

#include "AST/utils/macro.h"
namespace Hzcc::AST {
ImplicitCastExpr::ImplicitCastExpr(std::string name,               // NOLINT
                                   const Position& location,       // NOLINT
                                   std::unique_ptr<ASTNode> expr)  // NOLINT
    : CastExpr(location, std::move(expr)), _cast_name(std::move(name)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(!_cast_name.empty())
        << HZCC_AST_PRINT_NODE_INFO("cast name is empty string", this)
    HZCC_RUNTIME_CHECK(GetCastExpr()->RetType() != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("cast expr have a nullptr type",
                                           this)
}

const char* ImplicitCastExpr::NodeName() const { return "ImplicitCastExpr"; }

void ImplicitCastExpr::PrintDetail(std::ostream& out,
                                   const std::string& ident) const {
    out << '[' << GetCastExpr()->RetType()->Name() << "]->[" << RetType()->Name()
        << "] <" << _cast_name << ">";
    GetCastExpr()->Dump(out, ident + " `");
}

}  // namespace Hzcc::AST
