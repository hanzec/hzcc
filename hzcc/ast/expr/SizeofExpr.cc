//
// Created by chen_ on 2022/3/29.
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
#include "enums.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc::ast {
SizeofExpr::SizeofExpr(const Position& location, std::unique_ptr<Stmt> expr)
    : Expr(location, "SizeofExpr"), _expr(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _expr != nullptr)
        << UniqueName() << "cast is nullptr";
#endif
}

std::shared_ptr<Type> SizeofExpr::retType() const {
    return GetNumericalTypeOf<PrimitiveType::kLong>();
}
Status SizeofExpr::visit(Visitor& visitor) { return visitor.visit(this); }

}  // namespace hzcc::ast
