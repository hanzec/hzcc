//
// Created by chen_ on 2022/3/29.
//
#include "for.h"
namespace Hzcc::AST {
const char* ForStatement::GetNodeName() const { return "ForStatement"; }

Status ForStatement::visit(ASTVisitor& visitor) { return visitor.visit(this); }

#ifdef NDEBUG
std::string ForStatement::Dump(std::string_view ident) const {
    std::string ret;

    // print parameter
    ret += _body->Dump(ident) + "\n";
    return ret;
}
#endif
}  // namespace Hzcc::AST
