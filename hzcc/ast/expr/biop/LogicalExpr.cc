//
// Created by Hanze Chen on 2022/3/29.
//
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
LogicalExpr::LogicalExpr(const Position& loc,        // NOLINT
                         std::string_view type,      // NOLINT
                         std::unique_ptr<Expr> lhs,  // NOLINT
                         std::unique_ptr<Expr> rhs)  // NOLINT
    : OperatorBase(loc, "LogicalExpr", std::move(lhs), std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-oop50-cpp"
    INTERNAL_LOG_IF(FATAL, !type.empty())
        << UniqueName() << "type string empty";
    INTERNAL_LOG_IF(FATAL, type.size() == 2)
        << UniqueName() << "type len mismatch";
#pragma clang diagnostic pop
#endif

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    if (type[0] == '&') {
        this->_type = kLogicalType_And;
    } else if (type[0] == '|') {
        this->_type = kLogicalType_Or;
    } else {
        INTERNAL_LOG(FATAL)
            << UniqueName()
            << "type: [" + std::string(type) + "] not supported";
    }
}

std::shared_ptr<Type> LogicalExpr::retType() const {
    return GetNumericalTypeOf<PrimitiveType::kChar>();
}
Status LogicalExpr::visit(Visitor& visitor) { return visitor.visit(this); }
LogicalType LogicalExpr::op_type() const { return _type; }

}  // namespace hzcc::ast
