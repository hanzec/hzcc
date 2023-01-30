//
// Created by Hanze Chen on 2022/3/29.
//
#include <glog/logging.h>

#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "ast/Stmt.h"
#include "ast/expr/Expr.h"
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
            this->_type = AssignType::ASSIGN;
            break;
        case '+':
            this->_type = AssignType::ADD;
            break;
        case '-':
            this->_type = AssignType::SUB;
            break;
        case '*':
            this->_type = AssignType::MUL;
            break;
        case '/':
            this->_type = AssignType::DIV;
            break;
        case '%':
            this->_type = AssignType::MOD;
            break;
        case '<':
            this->_type = AssignType::LSHIFT;
            break;
        case '>':
            this->_type = AssignType::RSHIFT;
            break;
        case '&':
            this->_type = AssignType::AND;
            break;
        case '|':
            this->_type = AssignType::OR;
            break;
        case '^':
            this->_type = AssignType::XOR;
            break;
        default:
            INTERNAL_LOG(FATAL)
                << UniqueName()
                << "type: [" + std::string(type) + "] not supported";
    }
};
}  // namespace hzcc::ast
