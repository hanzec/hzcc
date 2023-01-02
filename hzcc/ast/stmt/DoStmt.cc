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
DoStmt::DoStmt(const Position& pos, std::unique_ptr<Stmt> cond,
               std::unique_ptr<Stmt> body)  // NOLINT
    : Stmt(pos, "DoStmt"), _cond(std::move(cond)), _body(std::move(body)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _body != nullptr)
        << UniqueName() << "body is nullptr";
    INTERNAL_LOG_IF(FATAL, _cond != nullptr)
        << UniqueName() << "condition is nullptr";
#endif
}

Status DoStmt::visit(Visitor& visitor) { return visitor.visit(this); }
std::unique_ptr<Stmt>& DoStmt::cond_stmt() { return _cond; }
std::unique_ptr<Stmt>& DoStmt::body_stmt() { return _body; }
}  // namespace hzcc::ast
