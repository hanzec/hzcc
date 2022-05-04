//
// Created by chen_ on 2022/3/29.
//
#include "empty.h"

#include "AST/type/Type.h"

namespace Hzcc::AST {

const char* AST::EmptyStatement::GetNodeName() const {
    return "EmptyStatement";
}
std::shared_ptr<Type> EmptyStatement::GetType() const {
    return Type::GetBasicType("void", {Lexical::TokenType::kConst});
}
Status EmptyStatement::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
EmptyStatement::EmptyStatement(std::pair<int, int> pair_1) : ASTNode(pair_1) {}
}  // namespace Hzcc::AST
