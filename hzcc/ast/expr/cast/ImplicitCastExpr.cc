//
// Created by chen_ on 2022/6/13.
//

#include <utility>

#include "CastExpr.h"
namespace hzcc::ast {
ImplicitCastExpr::ImplicitCastExpr(std::string name,            // NOLINT
                                   const Position& location,    // NOLINT
                                   std::unique_ptr<Expr> expr)  // NOLINT
    : CastExpr("ImplicitCastExpr", location, std::move(expr)),
      _name(std::move(name)){
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, !_name.empty())
        << UniqueName() << "cast name is empty string";
    INTERNAL_LOG_IF(FATAL, GetCastExpr()->retType() != nullptr)
        << UniqueName() << "cast cast have a nullptr type";
#endif
}

void ImplicitCastExpr::PrintDetail(std::ostream& out,
                                   const std::string& ident) const {
    out << '[' << GetCastExpr()->retType()->Name() << "]->["
        << retType()->Name() << "] <" << _name << ">";
    GetCastExpr()->Dump(out, ident + " `");
}

}  // namespace hzcc::ast
