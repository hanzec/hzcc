//
// Created by chen_ on 2022/3/29.
//
#include <utility>

#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
VarDecl::VarDecl(const Position& loc, std::string_view name,
                 std::unique_ptr<Expr> init,
                 std::unique_ptr<TypeProxyExpr> type)  // NOLINT
    : IDeclStmt("VarDecl",name, loc),
      _type(std::move(type)),
      _init_expr(std::move(init)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK_BLOCK({
        // name and type is checked in DeclStmt
        INTERNAL_LOG_IF(FATAL, _init_expr != nullptr)
            << UniqueName() << "init expression is nullptr";
    })
}
Status VarDecl::visit(Visitor& visitor) { return visitor.visit(this); }

bool VarDecl::has_init() const { return _init_expr != nullptr; }

std::unique_ptr<Expr>& VarDecl::init_expr() {
    DLOG_ASSERT(_init_expr != nullptr) << "init cast is nullptr";
    return _init_expr;
}
}  // namespace hzcc::ast
