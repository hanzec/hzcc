//
// Created by chen_ on 2022/3/28.
//
#include "compound.h"

namespace Mycc::AST {

CompoundStmt::CompoundStmt(std::pair<int, int> loc) : ASTNode(loc) {}

bool CompoundStmt::AddStatement(std::unique_ptr<ASTNode> statement) {
    if (statement == nullptr) {
        return false;
    }
    statements_.push_back(std::move(statement));
    return true;
}
std::string CompoundStmt::GetNodeName() const { return "CompoundStmt"; }

const std::unique_ptr<ASTNode>& CompoundStmt::GetLastStatement() const {
    return statements_.back();
}

std::string CompoundStmt::PrintAdditionalInfo(std::string_view ident) const {
    std::string ret = "\n";
    for (const auto& statement : statements_) {
        ret += statement->Dump(std::string(ident.size(), ' ') + "  |") + "\n";
    }

    return ret;
}

#ifdef NDEBUG
std::string CompoundStmt::Dump(std::string_view ident) const {
    std::string ret;

    // print parameter
    for (const auto& arg : statements_) {
        ret += arg->Dump(ident) + "\n";
    }
    return ret;
}
#endif

}  // namespace Mycc::AST