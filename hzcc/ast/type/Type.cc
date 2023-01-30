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
    for (size_t i = 0; i < magic_enum::enum_count<Attribute>(); ++i) {
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

}  // namespace hzcc::ast
