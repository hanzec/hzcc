//
// Created by chen_ on 2022/3/26.
//
#include "literal.h"

#include <string>

#include "AST/DeduceValue.h"
#include "AST/type/ArrayType.h"
#include "AST/type/Type.h"
#include "lexical/Token.h"
namespace Mycc::AST {

std::string LiteralExpr::GetNodeName() const {
    switch (_type) {
        case ValueType::kChar:
            return "CharLiteral";
        case ValueType::kReal_number:
            return "RealNumberLiteral";
        case ValueType::kString:
            return "StringLiteral";
        case ValueType::kInteger:
            return "IntegerLiteral";
    }
    return "";
}

std::optional<DeduceValue> LiteralExpr::GetDeducedValue() const {
    switch (_type) {
        case ValueType::kChar:
            return DeduceValue((uint64_t) static_cast<int>(_value[0]));
        case ValueType::kReal_number:
            return DeduceValue(std::stod(_value));
        case ValueType::kString:
            return std::nullopt;
        case ValueType::kInteger:
            return DeduceValue((uint64_t)std::stoi(_value));
    }
    return std::nullopt;
}

std::string LiteralExpr::PrintAdditionalInfo(std::string_view ident) const {
    switch (_type) {
        case ValueType::kChar:
            return "char \"" + _value + "\"";
        case ValueType::kReal_number:
            return "real_number " + _value;
        case ValueType::kString:
            return "string \"" + _value + "\"";
        case ValueType::kInteger:
            return "int " + _value;
    }
    return "";
}
LiteralExpr::LiteralExpr(int64_t value)
    : ASTNode({-1, -1}), _type(kInteger), _value(std::to_string(value)) {}

LiteralExpr::LiteralExpr(LiteralExpr::ValueType type,
                         const Lexical::Token& value)
    : ASTNode(value.Location()), _type(type), _value(value.Value()) {}

std::shared_ptr<Type> LiteralExpr::GetType() const {
    static std::list<std::string> const_type_list = {"const"};

    switch (_type) {
        case ValueType::kChar:
            return Type::GetBasicType("char", {Lexical::TokenType::kConst});
        case ValueType::kReal_number:
            return Type::GetBasicType("double", {Lexical::TokenType::kConst});
        case ValueType::kString:
            return ArrayType::GetArrayOfBasicType(
                Type::GetBasicType("char", {}),
                std::make_unique<LiteralExpr>(_value.size() + 1),
                {Lexical::TokenType::kConst});
        case ValueType::kInteger:
            return Type::GetBasicType("int", {Lexical::TokenType::kConst});
    }
    return nullptr;
}

bool LiteralExpr::IsAssignable() const { return false; }
void LiteralExpr::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}

}  // namespace Mycc::AST
