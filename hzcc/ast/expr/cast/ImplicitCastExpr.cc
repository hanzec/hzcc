//
// Created by chen_ on 2022/6/13.
//

#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "ast/expr/Expr.h"
#include "ast/Stmt.h"
#include "macro.h"
#include "utils/logging.h"

namespace hzcc::ast {
ImplicitCastExpr::ImplicitCastExpr(std::string name,            // NOLINT
                                   const Position& location,    // NOLINT
                                   std::unique_ptr<Expr> expr)  // NOLINT
    : CastExpr("ImplicitCastExpr", location, std::move(expr)),
      _name(std::move(name)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, !_name.empty())
        << UniqueName() << "cast name is empty string";
    INTERNAL_LOG_IF(FATAL, cast_expr()->retType() != nullptr)
        << UniqueName() << "cast cast have a nullptr type";
#endif
}

}  // namespace hzcc::ast
