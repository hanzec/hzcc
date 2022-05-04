//
// Created by chen_ on 2022/3/29.
//
#include "unary.h"

#include <cassert>

#include "AST/type/Type.h"
#include "lexical/Token.h"
namespace Hzcc::AST {
const char* UnaryExpr::GetNodeName() const { return "UnaryExpr"; }
UnaryExpr::UnaryExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> expr)
    : ASTNode(type.Location()), _expr(std::move(expr)) {
    switch (type.Type()) {
        case Lexical::TokenType::kSub:
            _type = UnaryType::kUnaryMinus;
            break;
        case Lexical::TokenType::kLogicalNot:
            _type = UnaryType::kLogicalNot;
            break;
        case Lexical::TokenType::kReference:
            _type = UnaryType::kReference;
            break;
        case Lexical::TokenType::kBitWiseNot:
            _type = UnaryType::kBitwiseNot;
            break;
        case Lexical::TokenType::kDereference:
            _type = UnaryType::kDereference;
            break;
        case Lexical::TokenType::kSelfIncrement:
            _type = UnaryType::kPreInc;
            break;
        case Lexical::TokenType::kSelfDecrement:
            _type = UnaryType::kPreDec;
            break;
        default:
            DLOG(FATAL) << "UnaryExpr: Type [" << type.Type()
                        << "] not supported";
    }
}
std::shared_ptr<Type> UnaryExpr::GetType() const {
    if (_type == kLogicalNot)
        return Type::GetBasicType("char", {Lexical::TokenType::kConst});
    else
        return _expr->GetType();
}
Status UnaryExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
