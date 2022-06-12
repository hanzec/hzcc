//
// Created by chen_ on 2022/3/29.
//
#include "EmptyStmt.h"

#include "AST/type/Type.h"
namespace Hzcc::AST {
EmptyStmt::EmptyStmt() : ASTNode({0, 0}) {}

const char* AST::EmptyStmt::NodeName() const { return "EmptyStmt"; }
std::shared_ptr<Type> EmptyStmt::RetType() const {
    return Type::GetTypeOf("void", {Lexical::TokenType::kConst});
}
Status EmptyStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }

bool EmptyStmt::IsEmptyStmt() const { return true; }
}  // namespace Hzcc::AST
