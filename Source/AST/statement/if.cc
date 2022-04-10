
//
// Created by chen_ on 2022/3/29.
//
#include "if.h"
namespace Mycc::AST {
std::string IfStatement::GetNodeName() const { return "IfStatement"; }

void IfStatement::visit(ASTVisitor &visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}

#ifdef NDEBUG
std::string IfStatement::Dump(std::string_view ident) const {
    std::string ret;

    // print parameter
    ret += _if_body_statement->Dump(ident) + "\n";

    // dump else
    for (const auto &stmt : _elseIfs) {
        ret += stmt.second->Dump(ident) + "\n";
    }

    if (_else_statement_) {
        ret += _else_statement_->Dump(ident) + "\n";
    }

    return ret;
}
#endif

}  // namespace Mycc::AST
