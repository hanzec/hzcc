//
// Created by chen_ on 2022/3/29.
//
#include "EmptyStmt.h"

#include "AST/type/Type.h"
namespace Hzcc::AST {
EmptyStatement::EmptyStatement(const std::pair<int, int>& loc) : ASTNode(loc) {}

const char* AST::EmptyStatement::NodeName() const { return "EmptyStatement"; }
std::shared_ptr<Type> EmptyStatement::RetType() const {
    return Type::GetTypeOf("void", {Lexical::TokenType::kConst});
}
Status EmptyStatement::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}

bool EmptyStatement::IsEmptyStmt() const { return true; }
}  // namespace Hzcc::AST
