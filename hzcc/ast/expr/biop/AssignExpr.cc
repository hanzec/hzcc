//
// Created by Hanze Chen on 2022/3/29.
//
#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "ast/expr/Expr.h"
#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc::ast {
AssignExpr::AssignExpr(const Position& loc,        // NO_LINT
                       std::string_view type,      // NO_LINT
                       std::unique_ptr<Expr> lhs,  // NO_LINT
                       std::unique_ptr<Expr> rhs)  // NO_LINT
    : OperatorBase(loc, "AssignExpr", std::move(lhs), std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK

    INTERNAL_LOG_IF(FATAL, !type.empty())
        << UniqueName() << "type string empty";
    INTERNAL_LOG_IF(
        FATAL, (type.length() == 2 && type[0] != '=') ||
                   (type.length() == 1 && type[0] == '=') ||
                   (type.length() == 3 && (type[0] == '<' || type[0] == '>')))
        << UniqueName() << "type len mismatch";
#endif

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '=':
            this->_type = AssignType::kAssignType_Assign;
            break;
        case '+':
            this->_type = AssignType::kAssignType_AddAssign;
            break;
        case '-':
            this->_type = AssignType::kAssignType_SubAssign;
            break;
        case '*':
            this->_type = AssignType::kAssignType_MulAssign;
            break;
        case '/':
            this->_type = AssignType::kAssignType_DivAssign;
            break;
        case '%':
            this->_type = AssignType::kAssignType_ModAssign;
            break;
        case '<':
            this->_type = AssignType::kAssignType_LShiftAssign;
            break;
        case '>':
            this->_type = AssignType::kAssignType_RShiftAssign;
            break;
        case '&':
            this->_type = AssignType::kAssignType_AndAssign;
            break;
        case '|':
            this->_type = AssignType::kAssignType_OrAssign;
            break;
        case '^':
            this->_type = AssignType::kAssignType_XorAssign;
            break;
        default:
            INTERNAL_LOG(FATAL)
                << UniqueName()
                << "type: [" + std::string(type) + "] not supported";
    }
};

Status AssignExpr::visit(Visitor& visitor) { return visitor.visit(this); }
AssignType AssignExpr::op_type() const { return _type; }

}  // namespace hzcc::ast
