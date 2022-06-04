//
// Created by chen_ on 2022/3/29.
//
#include "StructDeclStmt.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
StructDeclStmt::StructDeclStmt(const Position& loc,
                               const std::string_view& name,
                               std::shared_ptr<StructType> type)  // NO_LINT
    : DeclStmt(type, name, loc) {}

const char* StructDeclStmt::NodeName() const { return "StructDeclStmt"; }

Status StructDeclStmt::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
}  // namespace Hzcc::AST
