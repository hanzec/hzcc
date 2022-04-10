//
// Created by chen_ on 2022/3/29.
//
#include "empty.h"

#include "AST/type/Type.h"

namespace Mycc::AST {

std::string AST::EmptyStatement::GetNodeName() const {
    return "EmptyStatement";
}
std::shared_ptr<Type> EmptyStatement::GetType() const {
    return Type::GetBasicType("void", {Lexical::TokenType::kConst});
}
void EmptyStatement::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}
}  // namespace Mycc::AST