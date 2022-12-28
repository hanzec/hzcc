//
// Created by chen_ on 2022/3/29.
//
#include <utility>

#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
VarDecl::VarDecl(const Position& loc, std::string_view name,
                 std::shared_ptr<Type> type,
                 std::unique_ptr<Expr> init)  // NOLINT
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

void VarDecl::PrintDetail(std::ostream& out, const std::string& ident) const {
    out << DeclName() << " " << declType()->Name();
    if (HasInitExpr()) _init_expr->Dump(out, ident + " `");
}
Status VarDecl::visit(Visitor& visitor) { return visitor.visit(this); }

bool VarDecl::HasInitExpr() const { return _init_expr != nullptr; }

std::unique_ptr<Expr>& VarDecl::GetInitExpr() {
    DLOG_ASSERT(_init_expr != nullptr) << "init cast is nullptr";
    return _init_expr;
}
}  // namespace hzcc::ast
