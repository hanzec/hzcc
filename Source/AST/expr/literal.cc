//
// Created by chen_ on 2022/3/26.
//
#include "literal.h"

#include <string>

#include "AST/DeduceValue.h"
#include "AST/type/ArrayType.h"
#include "AST/type/Type.h"
#include "lexical/Token.h"
namespace Hzcc::AST {

const char* LiteralExpr::GetNodeName() const {
    switch (_type) {
        case LiteralType::kLiteralType_Char:
            return "CharLiteral";
        case LiteralType::kLiteralType_Real_number:
            return "RealNumberLiteral";
        case LiteralType::kLiteralType_String:
            return "StringLiteral";
        case LiteralType::kLiteralType_Integer:
            return "IntegerLiteral";
        default:
            DLOG_ASSERT(false) << "unexpected literal type";
    }
    return "";
}

std::optional<DeduceValue> LiteralExpr::GetDeducedValue() const {
    switch (_type) {
        case LiteralType::kLiteralType_Char:
            return DeduceValue((uint64_t) static_cast<int>(_value[0]));
        case LiteralType::kLiteralType_Real_number:
            return DeduceValue(std::stod(_value));
        case LiteralType::kLiteralType_String:
            return std::nullopt;
        case LiteralType::kLiteralType_Integer:
            return DeduceValue((uint64_t)std::stoi(_value));
        default:
            DLOG_ASSERT(false) << "unexpected literal type";
    }
    return std::nullopt;
}

std::string LiteralExpr::PrintAdditionalInfo(std::string_view ident) const {
    switch (_type) {
        case LiteralType::kLiteralType_Char:
            return "char \"" + _value + "\"";
        case LiteralType::kLiteralType_Real_number:
            return "real_number " + _value;
        case LiteralType::kLiteralType_String:
            return "string \"" + _value + "\"";
        case LiteralType::kLiteralType_Integer:
            return "int " + _value;
    }
    return "";
}
LiteralExpr::LiteralExpr(int64_t value)
    : ASTNode({-1, -1}),
      _type(kLiteralType_Integer),
      _value(std::to_string(value)) {}

LiteralExpr::LiteralExpr(LiteralType type, const Lexical::Token& value)
    : ASTNode(value.Location()), _type(type), _value(value.Value()) {}

std::shared_ptr<Type> LiteralExpr::GetType() const {
    static std::list<std::string> const_type_list = {"const"};

    switch (_type) {
        case LiteralType::kLiteralType_Char:
            return Type::GetBasicType("char", {Lexical::TokenType::kConst});
        case LiteralType::kLiteralType_Real_number:
            return Type::GetBasicType("double", {Lexical::TokenType::kConst});
        case LiteralType::kLiteralType_String:
            return ArrayType::GetArrayOfBasicType(
                Type::GetBasicType("char", {}),
                std::make_unique<LiteralExpr>(_value.size() + 1),
                {Lexical::TokenType::kConst});
        case LiteralType::kLiteralType_Integer:
            return Type::GetBasicType("int", {Lexical::TokenType::kConst});
        default:
            DLOG_ASSERT(false) << "unexpected literal type";
    }
    return nullptr;
}

bool LiteralExpr::IsAssignable() const { return false; }
Status LiteralExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }

}  // namespace Hzcc::AST
