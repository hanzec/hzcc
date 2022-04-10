//
// Created by chen_ on 2022/3/29.
//
#include "operator.h"

#include "AST/type/Type.h"
namespace Mycc::AST {

std::string OperatorBase::GetNodeName() const { return "OperatorBase"; }
const std::unique_ptr<ASTNode>& OperatorBase::GetLHS() const { return _lhs; }
const std::unique_ptr<ASTNode>& OperatorBase::GetRHS() const { return _rhs; }
std::shared_ptr<Type> OperatorBase::GetType() const {
    if (_lhs->GetType()->IsConst()) {
        return _lhs->GetType();
    } else {
        return _rhs->GetType();
    }
}
}  // namespace Mycc::AST