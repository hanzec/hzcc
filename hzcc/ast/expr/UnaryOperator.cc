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
    LOG_IF(FATAL, _expr != nullptr) << UniqueName() << "cast is nullptr";
    LOG_IF(FATAL, !type.empty()) << UniqueName() << "type is empty string";
#endif

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '-':
            if (type.size() == 1) {
                _type = UnaryType::SUB;
            } else {
                if (loc.first < _expr->loc().first ||
                    loc.second < _expr->loc().second) {
                    _type = UnaryType::PRE_DEC;
                } else {
                    _type = UnaryType::POST_DEC;
                }
            }
            break;
        case '+':
            if (type.size() == 1) {
                _type = UnaryType::ADD;
            } else {
                if (loc.first < _expr->loc().first ||
                    loc.second < _expr->loc().second) {
                    _type = UnaryType::PRE_INC;
                } else {
                    _type = UnaryType::POST_INC;
                }
            }
            break;
        case '&':
            _type = UnaryType::ADDR;
            break;
        case '*':
            _type = UnaryType::DEREF;
            break;
        case '!':
            _type = UnaryType::NOT;
            break;
        case '~':
            _type = UnaryType::BIT_NOT;
            break;
        default:
            LOG(FATAL) << UniqueName()
                       << "type: [" + std::string(type) + "] not supported ";
    }
}
}  // namespace hzcc::ast
