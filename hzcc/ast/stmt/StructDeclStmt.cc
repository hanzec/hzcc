//
// Created by chen_ on 2022/3/29.
//
#include <memory>
#include <string_view>
#include <utility>

#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/status/status.h"

namespace hzcc::ast {
RecordDecl::RecordDecl(const Position& loc,
                       std::string_view decl_name,
                       std::unique_ptr<TypeProxyExpr> type)  // NO_LINT
    : IDeclStmt("StructDeclStmt",decl_name, loc),
      _type(std::move(type)) {}

Status RecordDecl::visit(Visitor& visitor) { return visitor.visit(this); }
}  // namespace hzcc::ast
