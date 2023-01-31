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
}  // namespace hzcc::ast
