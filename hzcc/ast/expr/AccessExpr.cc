//
// Created by Hanze Chen on 2022/3/29.
//
#include "Expr.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
AccessExpr::AccessExpr(bool isPtr,                  // NOLINT
                       const Position& loc,         // NOLINT
                       std::string_view field,      // NOLINT
                       std::unique_ptr<Expr> expr)  // NOLINT
    : Expr(loc, "AccessExpr"),
      _ptr_access(isPtr),
      _field(std::string(field)),
      _varaible(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, !field.empty())
        << UniqueName() << "field string empty";
    INTERNAL_LOG_IF(FATAL, _varaible != nullptr)
        << UniqueName() << "chain_access is nullptr";
    INTERNAL_LOG_IF(FATAL, _varaible->retType()->IsStruct())
        << UniqueName() << "chain_access is not struct";
#endif
}

bool AccessExpr::IsReturnLValue() const { return true; }

std::shared_ptr<Type> AccessExpr::retType() const {
    return dynamic_cast<StructType*>(_varaible->retType().get())
        ->ChildType(_field);
}
Status AccessExpr::visit(Visitor& visitor) { return visitor.visit(this); }
}  // namespace hzcc::ast
