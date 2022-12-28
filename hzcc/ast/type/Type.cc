//
// Created by chen_ on 2022/3/27.
//
#include "ast/type/Type.h"

#include <magic_enum.hpp>
#include <string>

#include "parser/common/name_utils.h"
#include "parser/lexical/utils/keywords_utils.h"
#include "utils/logging.h"
#include "utils/type_name_utils.h"
namespace hzcc::ast {
Type::Type(TypeCategory typeCategory, const std::list<Attribute>& attrs)
    : _typeCategory(typeCategory) {
    // check attrs
    for (const auto& attr : attrs) {
        // the attribute should also be a valid attribute
        DLOG_ASSERT(parser_common::IsAttribute(attr))
            << " invalid attribute id: " << magic_enum::enum_integer(attr);
    }

    // setting attrs
    for (auto attr : attrs) {
        _attrs[magic_enum::enum_integer(attr)] = true;
    }
}

bool Type::IsArray() const { return false; }

bool Type::IsStruct() const { return false; }

bool Type::IsFuncPtr() const { return false; }

bool Type::IsNumericalType() const { return false; }

std::string Type::Dump() { return Name(); }

std::list<Attribute> Type::GetAttributes() {
    std::list<Attribute> ret;
    for (size_t i = 0; i < parser_common::kAttributeTableSize; ++i) {
        if (_attrs[i]) {
            ret.push_back(static_cast<Attribute>(i));
        }
    }
    return ret;
}
bool Type::operator==(const Type& rhs) const { return IsSame(rhs); }
bool Type::operator!=(const Type& type) const { return !IsSame(type); }
bool Type::is_ptr() const { return false; }
uintptr_t Type::Id() const { return reinterpret_cast<uintptr_t>(this); }

std::string Type::UniqueName() const { return "[" + Name() + "]"; }
static TypePtr GetNumericalTypeOf(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::kInt:
            return GetNumericalTypeOf<PrimitiveType::kInt>();
        case PrimitiveType::kChar:
            return GetNumericalTypeOf<PrimitiveType::kChar>();
        case PrimitiveType::kFloat:
            return GetNumericalTypeOf<PrimitiveType::kFloat>();
        case PrimitiveType::kDouble:
            return GetNumericalTypeOf<PrimitiveType::kDouble>();
        case PrimitiveType::kVoid:
            return GetNumericalTypeOf<PrimitiveType::kVoid>();
        case PrimitiveType::kLong:
            return GetNumericalTypeOf<PrimitiveType::kLong>();
        case PrimitiveType::kShort:
            return GetNumericalTypeOf<PrimitiveType::kShort>();
        case PrimitiveType::kBool:
            return GetNumericalTypeOf<PrimitiveType::kBool>();
        case PrimitiveType::kComplex:
            return GetNumericalTypeOf<PrimitiveType::kComplex>();
        case PrimitiveType::kImaginary:
            return GetNumericalTypeOf<PrimitiveType::kImaginary>();
        case PrimitiveType::kLonglong:
            return GetNumericalTypeOf<PrimitiveType::kLonglong>();
        case PrimitiveType::kLong_double:
            return GetNumericalTypeOf<PrimitiveType::kLong_double>();
        default:
            INTERNAL_LOG(ERROR)
                << "Unknown primitive type: " << utils::as_integer(type);
    }
    INTERNAL_LOG(ERROR) << "Unknown primitive type: "
                        << utils::as_integer(type);
    exit(1);
}
}  // namespace hzcc::ast
