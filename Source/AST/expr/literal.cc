//
// Created by chen_ on 2022/3/26.
//
#include "literal.h"

#include <string>

#include "AST/type/array_type.h"
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

bool LiteralExpr::IsDeducible() const { return _type != kString; }
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
LiteralExpr::LiteralExpr(LiteralExpr::ValueType type,
                         const Lexical::Token& value)
    : ASTNode(value.Location()), _type(type), _value(value.Value()) {}

std::shared_ptr<Type> LiteralExpr::GetType() const {
    static std::list<std::string> const_type_list = {"const"};

    switch (_type) {
        case ValueType::kChar:
            return std::make_shared<Type>("char");
        case ValueType::kReal_number:
            return std::make_shared<Type>("double");
        case ValueType::kString:
            return std::make_shared<ArrayType>(std::make_shared<Type>("char"),
                                               _value.size());
        case ValueType::kInteger:
            return std::make_shared<Type>("int");
    }
    return nullptr;
}

bool LiteralExpr::IsAssignable() const { return false; }

}  // namespace Mycc::AST