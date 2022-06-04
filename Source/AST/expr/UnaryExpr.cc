//
// Created by chen_ on 2022/3/29.
//
#include "UnaryExpr.h"

#include <algorithm>

#include "AST/type/Type.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
constexpr static std::array<const char*, kUnaryType_ENUM_SIZE> kUnaryOpSTR = {
    "-", "++(Pre)", "--(Pre)", "++(Post)", "--(Post)", "&", "*", "!", "~"};

UnaryExpr::UnaryExpr(const std::string_view& type,
                     std::unique_ptr<ASTNode> expr,
                     const std::pair<int, int>& location)
    : ASTNode(location), _expr(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_expr != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("expr is nullptr", this);
    HZCC_RUNTIME_CHECK(!type.empty())
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("type is empty string", this);

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
            HZCC_RUNTIME_CHECK(false) << HZCC_AST_PRINT_CHECK_ERROR_INFO(
                "RetType: [" + std::string(type) + "] not supported ", this);
    }
}

const char* UnaryExpr::NodeName() const { return "UnaryExpr"; }

std::shared_ptr<Type> UnaryExpr::RetType() const {
    if (_type == kUnaryType_LogicalNot)
        return Type::GetTypeOf("char", {Lexical::TokenType::kConst});
    else
        return _expr->RetType();
}
Status UnaryExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }

std::string UnaryExpr::PrintDetail(const std::string& ident) const {
    std::stringstream ss;

    // unary op info
    ss << kUnaryOpSTR[_type] << " " << _expr->RetType()->GetName();

    // sub expr info
    std::string new_ident(ident);
    std::replace(new_ident.begin(), new_ident.end(), '`', ' ');
    ss << std::endl << _expr->Dump(new_ident + "`");

    return ss.str();
}
std::unique_ptr<ASTNode>& UnaryExpr::GetExpr() { return _expr; }
UnaryType UnaryExpr::GetUnaryType() const { return _type; }
}  // namespace Hzcc::AST
