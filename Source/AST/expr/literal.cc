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

const char* LiteralExpr::NodeName() const {
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
            return DeduceValue(_value[0]);
        case LiteralType::kLiteralType_Real_number:
            return DeduceValue(std::stod(_value));
        case LiteralType::kLiteralType_String:
            return std::nullopt;
        case LiteralType::kLiteralType_Integer:
            return DeduceValue(static_cast<int64_t>(std::stoi(_value)));
        default:
            DLOG_ASSERT(false) << "unexpected literal type";
    }
    return std::nullopt;
}

std::string LiteralExpr::PrintAdditionalInfo(const std::string& ident) const {
    switch (_type) {
        case LiteralType::kLiteralType_Char:
            return "char \'" + _value + "\'";
        case LiteralType::kLiteralType_Real_number:
            return "real_number " + _value;
        case LiteralType::kLiteralType_String:
            return "string \"" + _value + "\"";
        case LiteralType::kLiteralType_Integer:
            return "int " + _value;
        case kLiteralType_Max:
            DLOG_ASSERT(false) << "unexpected literal type [kLiteralType_Max]";
    }
    return "";
}
LiteralExpr::LiteralExpr(int64_t value)
    : ASTNode({-1, -1}),
      _type(kLiteralType_Integer),
      _value(std::to_string(value)) {}

LiteralExpr::LiteralExpr(enum LiteralType type, const Lexical::Token& value)
    : ASTNode(value.Location()),
      _type(type),
      _value(value.Value(true) +
             (type == LiteralType::kLiteralType_String ? "\\x00" : "")) {}

std::shared_ptr<Type> LiteralExpr::GetType() const {
    static std::list<std::string> const_type_list = {"const"};
    switch (_type) {
        case LiteralType::kLiteralType_Char:
            return Type::GetBasicType("char", {Lexical::TokenType::kConst});
        case LiteralType::kLiteralType_Real_number:
            return Type::GetBasicType("float", {Lexical::TokenType::kConst});
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
LiteralType LiteralExpr::LiteralType() const { return _type; }
bool LiteralExpr::IsLiteral() const { return true; }
const std::string& LiteralExpr::GetValue() const { return _value; }

}  // namespace Hzcc::AST
