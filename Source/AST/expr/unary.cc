//
// Created by chen_ on 2022/3/29.
//
#include "unary.h"

#include <algorithm>
#include <cassert>

#include "AST/type/Type.h"
#include "lexical/Token.h"
namespace Hzcc::AST {
constexpr static std::array<const char*, kUnaryType_ENUM_SIZE> kUnaryOpSTR = {
    "-", "++(Pre)", "--(Pre)", "++(Post)", "--(Post)", "&", "*", "!", "~"};

const char* UnaryExpr::GetNodeName() const { return "UnaryExpr"; }
UnaryExpr::UnaryExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> expr)
    : ASTNode(type.Location()), _expr(std::move(expr)) {
    switch (type.Type()) {
        case Lexical::TokenType::kSub:
            _type = kUnaryType_UnaryMinus;
            break;
        case Lexical::TokenType::kLogicalNot:
            _type = kUnaryType_LogicalNot;
            break;
        case Lexical::TokenType::kReference:
            _type = kUnaryType_Reference;
            break;
        case Lexical::TokenType::kBitWiseNot:
            _type = kUnaryType_BitwiseNot;
            break;
        case Lexical::TokenType::kDereference:
            _type = kUnaryType_Dereference;
            break;
        case Lexical::TokenType::kSelfIncrement:
            if (type.Location().first < _expr->Location().first ||
                type.Location().second < _expr->Location().second) {
                _type = kUnaryType_PreInc;
            } else {
                _type = kUnaryType_PostInc;
            }
            break;
        case Lexical::TokenType::kSelfDecrement:
            if (type.Location().first < _expr->Location().first ||
                type.Location().second < _expr->Location().second) {
                _type = kUnaryType_PreDec;
            } else {
                _type = kUnaryType_PostDec;
            }
            break;
        default:
            DLOG(FATAL) << "UnaryExpr: Type [" << type.Type()
                        << "] not supported";
    }
}
std::shared_ptr<Type> UnaryExpr::GetType() const {
    if (_type == kUnaryType_LogicalNot)
        return Type::GetBasicType("char", {Lexical::TokenType::kConst});
    else
        return _expr->GetType();
}
Status UnaryExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }

std::string UnaryExpr::PrintAdditionalInfo(std::string_view ident) const {
    std::stringstream ss;

    // unary op info
    ss << kUnaryOpSTR[_type] << " " << _expr->GetType()->GetName();

    // sub expr info
    std::string new_ident(ident);
    std::replace(new_ident.begin(), new_ident.end(), '`', ' ');
    ss << std::endl << _expr->Dump(new_ident + "`");

    return ss.str();
}
std::unique_ptr<ASTNode>& UnaryExpr::GetExpr() { return _expr; }
UnaryType UnaryExpr::GetUnaryType() const { return _type; }
}  // namespace Hzcc::AST
