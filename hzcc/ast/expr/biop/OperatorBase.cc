//
// Created by chen_ on 2022/3/29.
//
#include "OperatorBase.h"

#include "ast/DeduceValue.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
OperatorBase::OperatorBase(const Position& loc,        // NOLINT
                           const char* node_name,      // NOLINT
                           std::unique_ptr<Expr> lhs,  // NOLINT
                           std::unique_ptr<Expr> rhs)  // NOLINT
    : Expr(loc, node_name), _lhs(std::move(lhs)), _rhs(std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _lhs != nullptr) << UniqueName() << "lhs is nullptr";
    INTERNAL_LOG_IF(FATAL, _rhs != nullptr) << UniqueName() << "rhs is nullptr";
    INTERNAL_LOG_IF(FATAL, !_lhs->retType()->IsArray())
        << UniqueName() << "lhs could not be array";
    INTERNAL_LOG_IF(FATAL, !_rhs->retType()->IsArray())
        << UniqueName() << "rhs could not be array";
#endif
}

const std::unique_ptr<Expr>& OperatorBase::GetLHS() const { return _lhs; }
const std::unique_ptr<Expr>& OperatorBase::GetRHS() const { return _rhs; }

std::shared_ptr<Type> OperatorBase::retType() const {
    if (!_lhs->GetDeducedValue().has_value() &&
        !_rhs->GetDeducedValue().has_value()) {
        return _lhs->retType();
    } else {
        return _lhs->GetDeducedValue().has_value() ? _rhs->retType()
                                                   : _lhs->retType();
    }
}

}  // namespace hzcc::ast
