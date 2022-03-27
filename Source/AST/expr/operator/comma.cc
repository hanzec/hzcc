//
// Created by chen_ on 2022/3/29.
//
#include "comma.h"

#include "lexical/Token.h"
namespace Mycc::AST {

std::string AST::CommaExpr::GetNodeName() const { return "CommaExpr"; }
CommaExpr::CommaExpr(const Lexical::Token& token, std::unique_ptr<ASTNode> lhs,
                     std::unique_ptr<ASTNode> rhs)
    : OperatorBase(token.Location(), std::move(rhs), std::move(lhs)) {}
}  // namespace Mycc::AST