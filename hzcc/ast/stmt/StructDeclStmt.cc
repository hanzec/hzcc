//
// Created by chen_ on 2022/3/29.
//
#include "ast/Stmt.h"

namespace hzcc::ast {
RecordDecl::RecordDecl(const Position& loc,
                       std::string_view decl_name,
                               std::shared_ptr<StructType> type)  // NO_LINT
    : IDeclStmt("StructDeclStmt",decl_name, loc),
      _type(std::move(type)) {}

Status RecordDecl::visit(Visitor& visitor) { return visitor.visit(this); }
}  // namespace hzcc::ast
