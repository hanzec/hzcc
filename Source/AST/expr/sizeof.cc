//
// Created by chen_ on 2022/3/29.
//
#include "sizeof.h"

#include "AST/type/Type.h"

namespace Hzcc::AST {

std::string AST::SizeofExpr::GetNodeName() const { return "SizeofExpr"; }

std::shared_ptr<Type> SizeofExpr::GetType() const {
    return Type::GetBasicType("int", {Lexical::TokenType::kConst});
}
Status SizeofExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
