//
// Created by Hanze Chen on 2022/3/29.
//
#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "Expr.h"
#include "ast/type/Type.h"
#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc::ast {
MemberExpr::MemberExpr(bool isPtr,                  // NOLINT
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

bool MemberExpr::IsReturnLValue() const { return true; }

std::shared_ptr<Type> MemberExpr::retType() const {
    return dynamic_cast<StructType*>(_varaible->retType().get())
        ->field_type(_field);
}
Status MemberExpr::visit(Visitor& visitor) { return visitor.visit(this); }
}  // namespace hzcc::ast
