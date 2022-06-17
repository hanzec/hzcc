//
// Created by chen_ on 2022/6/13.
//
#include "BuiltinType.h"
namespace Hzcc::AST {
BuiltinType::BuiltinType(PrimitiveType type,                     // NOLINT
                         const std::list<TokenType>& attr_list)  // NOLINT
    : Type(attr_list), _type(type) {}

std::string BuiltinType::Name() const {
    return Keywords::kPrimitiveTypeTable[_type];
}

bool BuiltinType::IsBuiltinType() const { return true; }

bool BuiltinType::IsLongLong() const {
    return kPrimitiveType_longlong == _type;
}
bool BuiltinType::IsLongDouble() const {
    return kPrimitiveType_long_double == _type;
}
bool BuiltinType::IsVoid() const { return kPrimitiveType_void == _type; }
bool BuiltinType::IsLong() const { return kPrimitiveType_long == _type; }
bool BuiltinType::IsShort() const { return kPrimitiveType_short == _type; }
bool BuiltinType::IsChar() const { return kPrimitiveType_char == _type; }
bool BuiltinType::IsFloat() const { return kPrimitiveType_char == _type; }
bool BuiltinType::IsDouble() const { return kPrimitiveType_double == _type; }
bool BuiltinType::IsInteger() const { return kPrimitiveType_int == _type; }
uint8_t BuiltinType::GetTypeId() const { return _type; }

}  // namespace Hzcc::AST