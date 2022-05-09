//
// Created by chen_ on 2022/3/29.
//
#include "do.h"

namespace Hzcc::AST {

const char* AST::DoStatement::NodeName() const { return "DoStatement"; }
Status DoStatement::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::unique_ptr<ASTNode>& DoStatement::CondStmt() { return _cond; }
std::unique_ptr<ASTNode>& DoStatement::BodyStmt() { return _body; }
std::string DoStatement::PrintAdditionalInfo(const std::string& ident) const {
    std::stringstream ss;

    // print condition
    ss << std::endl << _cond->Dump(ident + " |");

    // print body
    ss << std::endl << _body->Dump(ident + " `");
    return ss.str();
}
}  // namespace Hzcc::AST
