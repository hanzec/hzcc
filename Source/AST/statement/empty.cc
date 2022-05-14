//
// Created by chen_ on 2022/3/29.
//
#include "empty.h"

#include "AST/type/Type.h"

namespace Hzcc::AST {

const char* AST::EmptyStatement::NodeName() const { return "EmptyStatement"; }
std::shared_ptr<Type> EmptyStatement::GetType() const {
    return Type::GetTypeOf("void", {Lexical::TokenType::kConst});
}
Status EmptyStatement::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
EmptyStatement::EmptyStatement(std::pair<int, int> pair_1) : ASTNode(pair_1) {}

bool EmptyStatement::IsEmptyStmt() const { return true; }
}  // namespace Hzcc::AST
