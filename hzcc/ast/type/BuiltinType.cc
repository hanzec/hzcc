//
// Created by chen_ on 2022/6/13.
//
#include <stdint.h>
#include <array>
#include <list>
#include <string>

#include "ast/type/Type.h"
#include "utils/constexpr_utils.h"
#include "enums.h"

namespace hzcc::ast {

NumericalType::NumericalType(
    PrimitiveType type,                                     // NOLINT
    const std::list<Attribute>& attr_list)  // NOLINT
    : Type(TypeCategory::kNumerical, attr_list), _type(type) {}

std::string NumericalType::Name() const {
    static constexpr int KPermittedTypeTableSize = 12;
    static constexpr std::array<const char*, KPermittedTypeTableSize>
        kPrimitiveTypeTable{"int",        "char",      "float",
                            "double",     "void",      "long",
                            "short",      "_Bool",     "_Complex",
                            "_Imaginary", "long long", "long double"};
    return kPrimitiveTypeTable[utils::as_integer(_type)];
}

bool NumericalType::IsNumericalType() const { return true; }

uint8_t NumericalType::GetTypeId() const { return utils::as_integer(_type); }
}  // namespace hzcc::ast