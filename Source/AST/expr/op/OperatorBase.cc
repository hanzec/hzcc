//
// Created by chen_ on 2022/3/29.
//
#include "OperatorBase.h"

#include "AST/DeduceValue.h"
#include "AST/type/Type.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
OperatorBase::OperatorBase(const Position& loc, std::unique_ptr<ASTNode> lhs,
                           std::unique_ptr<ASTNode> rhs)  // NOLINT
    : ASTNode(loc), _lhs(std::move(lhs)), _rhs(std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_lhs != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("lhs is nullptr", this);
    HZCC_RUNTIME_CHECK(_rhs != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("rhs is nullptr", this);
    HZCC_RUNTIME_CHECK(!_lhs->RetType()->IsArray())
        << HZCC_AST_PRINT_NODE_INFO("lhs could not be array", this);
    HZCC_RUNTIME_CHECK(!_rhs->RetType()->IsArray())
        << HZCC_AST_PRINT_NODE_INFO("rhs could not be array", this);
}

const char* OperatorBase::NodeName() const { return "OperatorBase"; }

const std::unique_ptr<ASTNode>& OperatorBase::GetLHS() const { return _lhs; }
const std::unique_ptr<ASTNode>& OperatorBase::GetRHS() const { return _rhs; }

std::shared_ptr<Type> OperatorBase::RetType() const {
    if (!_lhs->GetDeducedValue().has_value() &&
        !_rhs->GetDeducedValue().has_value()) {
        return _lhs->RetType();
    } else {
        return _lhs->GetDeducedValue().has_value() ? _rhs->RetType()
                                                   : _lhs->RetType();
    }
}

}  // namespace Hzcc::AST
