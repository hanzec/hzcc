//
// Created by chen_ on 2022/3/29.
//
#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "Expr.h"
#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "enums.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc::ast {
UnaryOperator::UnaryOperator(const Position& loc,    // NOLINT
                             std::string_view type,  // NOLINT
                             std::unique_ptr<Expr> expr)
    : Expr(loc, "UnaryOperator"), _expr(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _expr != nullptr)
        << UniqueName() << "cast is nullptr";
    INTERNAL_LOG_IF(FATAL, !type.empty())
        << UniqueName() << "type is empty string";
#endif

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '-':
            if (type.size() == 1) {
                _type = UnaryType::kUnaryMinus;
            } else {
                if (loc.first < _expr->loc().first ||
                    loc.second < _expr->loc().second) {
                    _type = UnaryType::kPreDec;
                } else {
                    _type = UnaryType::kPostDec;
                }
            }
            break;
        case '+':
            if (loc.first < _expr->loc().first ||
                loc.second < _expr->loc().second) {
                _type = UnaryType::kPreInc;
            } else {
                _type = UnaryType::kPostInc;
            }
            break;
        case '&':
            _type = UnaryType::kReference;
            break;
        case '*':
            _type = UnaryType::kDereference;
            break;
        case '!':
            _type = UnaryType::kLogicalNot;
            break;
        case '~':
            _type = UnaryType::kBitwiseNot;
            break;
        default:
            INTERNAL_LOG(FATAL)
                << UniqueName()
                << "retType: [" + std::string(type) + "] not supported ";
    }
}

Status UnaryOperator::visit(Visitor& visitor) { return visitor.visit(this); }

std::unique_ptr<Expr>& UnaryOperator::expr() { return _expr; }
UnaryType UnaryOperator::GetUnaryType() const { return _type; }
}  // namespace hzcc::ast
