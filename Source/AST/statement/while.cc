//
// Created by chen_ on 2022/3/29.
//
#include "while.h"
namespace Hzcc::AST {
std::string Hzcc::AST::WhileStatement::GetNodeName() const {
    return "WhileStatement";
}

Status WhileStatement::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}

#ifdef NDEBUG
std::string WhileStatement::Dump(std::string_view ident) const {
    std::string ret;

    // print parameter
    ret += body_->Dump(ident) + "\n";
    return ret;
}
#endif
}  // namespace Hzcc::AST
