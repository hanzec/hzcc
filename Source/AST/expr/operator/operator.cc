//
// Created by chen_ on 2022/3/29.
//
#include "operator.h"

namespace Mycc::AST {

std::string OperatorBase::GetNodeName() const { return "OperatorBase"; }
const std::unique_ptr<ASTNode>& OperatorBase::GetLHS() const { return _lhs; }
const std::unique_ptr<ASTNode>& OperatorBase::GetRHS() const { return _rhs; }
std::shared_ptr<Type> OperatorBase::GetType() const { return _lhs->GetType(); }
}  // namespace Mycc::AST