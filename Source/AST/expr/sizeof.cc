//
// Created by chen_ on 2022/3/29.
//
#include "sizeof.h"

#include "AST/type/Type.h"

namespace Mycc::AST {

std::string AST::SizeofExpr::GetNodeName() const { return "SizeofExpr"; }

std::shared_ptr<Type> SizeofExpr::GetType() const {
    return Type::GetBasicType("int", {Lexical::TokenType::kConst});
}
void SizeofExpr::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}
}  // namespace Mycc::AST
