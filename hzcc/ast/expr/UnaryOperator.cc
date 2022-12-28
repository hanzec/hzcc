//
// Created by chen_ on 2022/3/29.
//
#include <algorithm>

#include "ast/type/Type.h"
#include "Expr.h"
namespace hzcc::ast {
constexpr static std::array<const char*, kUnaryType_ENUM_SIZE> kUnaryOpSTR = {
    "-", "++(Pre)", "--(Pre)", "++(Post)", "--(Post)", "&", "*", "!", "~"};

UnaryOperator::UnaryOperator(const Position& loc, std::string_view type,
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
                _type = kUnaryType_UnaryMinus;
            } else {
                if (loc.first < _expr->Location().first ||
                    loc.second < _expr->Location().second) {
                    _type = kUnaryType_PreDec;
                } else {
                    _type = kUnaryType_PostDec;
                }
            }
            break;
        case '+':
            if (loc.first < _expr->Location().first ||
                loc.second < _expr->Location().second) {
                _type = kUnaryType_PreInc;
            } else {
                _type = kUnaryType_PostInc;
            }
            break;
        case '&':
            _type = kUnaryType_Reference;
            break;
        case '*':
            _type = kUnaryType_Dereference;
            break;
        case '!':
            _type = kUnaryType_LogicalNot;
            break;
        case '~':
            _type = kUnaryType_BitwiseNot;
            break;
        default:
            INTERNAL_LOG(FATAL)
                << UniqueName()
                << "retType: [" + std::string(type) + "] not supported ";
    }
}

Status UnaryOperator::visit(Visitor& visitor) { return visitor.visit(this); }

void UnaryOperator::PrintDetail(std::ostream& out,
                                const std::string& ident) const {
    out << kUnaryOpSTR[_type] << " " << _expr->retType()->Name();
    _expr->Dump(out, ident + "`");
}

std::unique_ptr<Expr>& UnaryOperator::GetExpr() { return _expr; }
UnaryType UnaryOperator::GetUnaryType() const { return _type; }
}  // namespace hzcc::ast
