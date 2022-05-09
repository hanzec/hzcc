//
// Created by chen_ on 2022/3/28.
//
#include "compound.h"

#include <regex>

namespace Hzcc::AST {

CompoundStmt::CompoundStmt(std::pair<int, int> loc) : ASTNode(loc) {}

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
std::string CompoundStmt::PrintAdditionalInfo(const std::string& ident) const {
    std::stringstream ret;

    ret << "\n";
    for (const auto& statement : statements_) {
        ret << statement->Dump(
                   ident + (statement == statements_.back() ? "  `" : "  |")) +
                   (statement == statements_.back() ? "" : "\n");
    }

    return ret.str();
}

#ifdef NDEBUG
std::string CompoundStmt::Dump(const std::string& ident) const {
    std::string ret;

    // print parameter
    for (const auto& arg : statements_) {
        ret += arg->Dump(ident) + (arg == statements_.back() ? "" : "\n");
    }
    return ret;
}
#endif

}  // namespace Hzcc::AST
