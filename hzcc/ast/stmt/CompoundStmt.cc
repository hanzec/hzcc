//
// Created by chen_ on 2022/3/28.
//
#include "ast/Stmt.h"
namespace hzcc::ast {

CompoundStmt::CompoundStmt(const Position& loc) : Stmt(loc, "CompoundStmt") {}

void CompoundStmt::AddStatement(std::unique_ptr<Stmt> statement) {
    LOG_IF(FATAL, statement == nullptr) << "statement is nullptr";
    statements_.push_back(std::move(statement));
}

const std::unique_ptr<Stmt>& CompoundStmt::GetLastStatement() const {
    return statements_.back();
}

std::list<std::unique_ptr<Stmt>>& CompoundStmt::GetBodyStatements() {
    return statements_;
}

Status CompoundStmt::visit(Visitor& visitor) { return visitor.visit(this); }
void CompoundStmt::PrintDetail(std::ostream& out,
                               const std::string& ident) const {
    for (const auto& statement : statements_) {
        statement->Dump(
            out, ident + (statement == statements_.back() ? "  `" : "  |"));
    }
}

}  // namespace hzcc::ast
