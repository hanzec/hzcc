//
// Created by chen_ on 2022/3/29.
//
#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>

#include "Expr.h"
#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc {
namespace ast {
class Type;
}  // namespace ast
}  // namespace hzcc

namespace hzcc::ast {
DeclRefExpr::DeclRefExpr(const Position& loc,    // NO_LINT
                         std::string_view name)  // NO_LINT
    : Expr(loc, "DeclRefExpr"), _name(std::string(name)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK_BLOCK(
        INTERNAL_LOG_IF(FATAL, _type != nullptr) << "type is nullptr";
        INTERNAL_LOG_IF(FATAL, !_name.empty()) << "name is empty string";)
}

std::shared_ptr<Type> DeclRefExpr::retType() const { return _type->retType(); }

bool DeclRefExpr::IsReturnLValue() const { return true; }
Status DeclRefExpr::visit(Visitor& visitor) { return visitor.visit(this); }
std::string_view DeclRefExpr::var_name() const { return _name; }

}  // namespace hzcc::ast
