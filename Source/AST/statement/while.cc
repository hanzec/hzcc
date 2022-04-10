//
// Created by chen_ on 2022/3/29.
//
#include "while.h"
namespace Mycc::AST {
std::string Mycc::AST::WhileStatement::GetNodeName() const {
    return "WhileStatement";
}

void WhileStatement::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}

#ifdef NDEBUG
std::string WhileStatement::Dump(std::string_view ident) const {
    std::string ret;

    // print parameter
    ret += body_->Dump(ident) + "\n";
    return ret;
}
#endif
}  // namespace Mycc::AST
