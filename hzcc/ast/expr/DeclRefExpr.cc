//
// Created by chen_ on 2022/3/29.
//
#include "Expr.h"
#include "ast/type/Type.h"

namespace hzcc::ast {
DeclRefExpr::DeclRefExpr(const Position& loc,         // NO_LINT
                         std::string_view name,       // NO_LINT
                         std::shared_ptr<Type> type)  // NO_LINT
    : Expr(loc, "DeclRefExpr"),
      _name(std::string(name)),
      _type(std::move(type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _type != nullptr) << "type is nullptr";
    INTERNAL_LOG_IF(FATAL, !_name.empty()) << "name is empty string";
#endif
}

std::shared_ptr<Type> DeclRefExpr::retType() const { return _type; }

void DeclRefExpr::PrintDetail(std::ostream& out,
                              const std::string& ident) const {
    out << _name << " " << _type->Name();
}
bool DeclRefExpr::IsReturnLValue() const { return true; }
Status DeclRefExpr::visit(Visitor& visitor) { return visitor.visit(this); }
const std::string& DeclRefExpr::VarName() const { return _name; }

}  // namespace hzcc::ast
