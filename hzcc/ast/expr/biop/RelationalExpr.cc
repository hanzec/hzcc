//
// Created by Hanze Chen on 2022/3/29.
//
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
RelationalExpr::RelationalExpr(const Position& loc, std::string_view type,
                               std::unique_ptr<Expr> lhs,
                               std::unique_ptr<Expr> rhs)  // NOLINT
    : OperatorBase(loc, "RelationalExpr", std::move(lhs), std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, !type.empty())
        << UniqueName() << "type string empty";
    INTERNAL_LOG_IF(FATAL, type.length() == 2)
        << UniqueName() << "type len mismatch";
#endif

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '=':
            this->_type = RelationalType::kRelationalType_Equal;
            break;
        case '!':
            this->_type = RelationalType::kRelationalType_NEqual;
            break;
        case '<':
            if (type[1] == '=')
                this->_type = RelationalType::kRelationalType_LessEqual;
            else
                this->_type = RelationalType::kRelationalType_Less;
            break;
        case '>':
            if (type[1] == '=')
                this->_type = RelationalType::kRelationalType_GreaterEqual;
            else
                this->_type = RelationalType::kRelationalType_Greater;
            break;
        default:
            INTERNAL_LOG(FATAL)
                << UniqueName()
                << "type: [" + std::string(type) + "] not supported";
    }
}

std::shared_ptr<Type> RelationalExpr::retType() const {
    return GetNumericalTypeOf<PrimitiveType::kChar>();
}
Status RelationalExpr::visit(Visitor& visitor) { return visitor.visit(this); }
RelationalType RelationalExpr::op_type() const { return _type; }
}  // namespace hzcc::ast
