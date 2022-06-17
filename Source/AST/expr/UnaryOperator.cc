//
// Created by chen_ on 2022/3/29.
//
#include "UnaryOperator.h"

#include <algorithm>

#include "AST/type/Type.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
constexpr static std::array<const char*, kUnaryType_ENUM_SIZE> kUnaryOpSTR = {
    "-", "++(Pre)", "--(Pre)", "++(Post)", "--(Post)", "&", "*", "!", "~"};

UnaryOperator::UnaryOperator(const Position& location,
                             const std::string_view& type,
                             std::unique_ptr<ASTNode> expr)
    : ASTNode(location), _expr(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_expr != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("expr is nullptr", this);
    HZCC_RUNTIME_CHECK(!type.empty())
        << HZCC_AST_PRINT_NODE_INFO("type is empty string", this);

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '-':
            if (type.size() == 1) {
                _type = kUnaryType_UnaryMinus;
            } else {
                if (location.first < _expr->Location().first ||
                    location.second < _expr->Location().second) {
                    _type = kUnaryType_PreDec;
                } else {
                    _type = kUnaryType_PostDec;
                }
            }
            break;
        case '+':
            if (location.first < _expr->Location().first ||
                location.second < _expr->Location().second) {
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
            HZCC_RUNTIME_CHECK(false) << HZCC_AST_PRINT_NODE_INFO(
                "RetType: [" + std::string(type) + "] not supported ", this);
    }
}

const char* UnaryOperator::NodeName() const { return "UnaryOperator"; }

std::shared_ptr<Type> UnaryOperator::RetType() const {
    if (_type == kUnaryType_LogicalNot)
        return GetCharType();
    else
        return _expr->RetType();
}
Status UnaryOperator::visit(ASTVisitor& visitor) { return visitor.visit(this); }

void UnaryOperator::PrintDetail(std::ostream& out,
                                const std::string& ident) const {
    out << kUnaryOpSTR[_type] << " " << _expr->RetType()->Name();
    _expr->Dump(out, ident + "`");
}

std::unique_ptr<ASTNode>& UnaryOperator::GetExpr() { return _expr; }
UnaryType UnaryOperator::GetUnaryType() const { return _type; }
}  // namespace Hzcc::AST
