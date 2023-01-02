//
// Created by chen_ on 2022/3/29.
//

#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc::ast {
ReturnStmt::ReturnStmt(const Position& loc,         // NOLINT
                       std::unique_ptr<Expr> expr)  // NOLINT
    : Stmt(loc, "ReturnStmt"), _ret_expr(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _ret_expr != nullptr)
        << UniqueName() << "return statement is nullptr";
#endif
}

Status ReturnStmt::visit(Visitor& visitor) { return visitor.visit(this); }
std::unique_ptr<Expr>& ReturnStmt::ret_expr() { return _ret_expr; }

}  // namespace hzcc::ast
