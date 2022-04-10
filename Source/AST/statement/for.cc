//
// Created by chen_ on 2022/3/29.
//
#include "for.h"
namespace Mycc::AST {
std::string ForStatement::GetNodeName() const { return "ForStatement"; }

void ForStatement::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}

#ifdef NDEBUG
std::string ForStatement::Dump(std::string_view ident) const {
    std::string ret;

    // print parameter
    ret += _body->Dump(ident) + "\n";
    return ret;
}
#endif
}  // namespace Mycc::AST
