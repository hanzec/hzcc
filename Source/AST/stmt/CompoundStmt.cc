//
// Created by chen_ on 2022/3/28.
//
#include "CompoundStmt.h"
namespace Hzcc::AST {

CompoundStmt::CompoundStmt(const Position& loc) : ASTNode(loc) {}

bool CompoundStmt::AddStatement(std::unique_ptr<ASTNode> statement) {
    if (statement == nullptr) {
        return false;
    }
    statements_.push_back(std::move(statement));
    return true;
}
const char* CompoundStmt::NodeName() const { return "CompoundStmt"; }

const std::unique_ptr<ASTNode>& CompoundStmt::GetLastStatement() const {
    return statements_.back();
}

std::list<std::unique_ptr<ASTNode>>& CompoundStmt::GetBodyStatements() {
    return statements_;
}

Status CompoundStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }
void CompoundStmt::PrintDetail(std::ostream& out,
                               const std::string& ident) const {
    for (const auto& statement : statements_) {
        statement->Dump(
            out, ident + (statement == statements_.back() ? "  `" : "  |"));
    }
}

}  // namespace Hzcc::AST
