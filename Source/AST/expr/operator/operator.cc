//
// Created by chen_ on 2022/3/29.
//
#include "operator.h"

#include "AST/DeduceValue.h"
#include "AST/type/Type.h"
#include "options.h"
namespace Hzcc::AST {

const char* OperatorBase::NodeName() const { return "OperatorBase"; }
const std::unique_ptr<ASTNode>& OperatorBase::GetLHS() const { return _lhs; }
const std::unique_ptr<ASTNode>& OperatorBase::GetRHS() const { return _rhs; }

std::shared_ptr<Type> OperatorBase::GetType() const {
    if (!_lhs->GetDeducedValue().has_value() &&
        !_rhs->GetDeducedValue().has_value()) {
        return _lhs->GetType();
    } else {
        return _lhs->GetDeducedValue().has_value() ? _rhs->GetType()
                                                   : _lhs->GetType();
    }
}
OperatorBase::OperatorBase(std::pair<int, int> loc,
                           std::unique_ptr<ASTNode> lhs,
                           std::unique_ptr<ASTNode> rhs)
    : ASTNode(loc), _lhs(std::move(lhs)), _rhs(std::move(rhs)) {
    DLOG_ASSERT(_lhs != nullptr) << " OperatorBase: lhs is nullptr";
    DLOG_ASSERT(_rhs != nullptr) << " OperatorBase: rhs is nullptr";
    DLOG_ASSERT(!Options::Global_enable_type_checking ||
                !_lhs->GetType()->IsArray())
        << " OperatorBase: rhs is array";
    DLOG_ASSERT(!Options::Global_enable_type_checking ||
                !_rhs->GetType()->IsArray())
        << " OperatorBase: rhs is array";
}
}  // namespace Hzcc::AST
