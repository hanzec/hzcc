//
// Created by chen_ on 2022/6/13.
//
#include <stdint.h>

#include <array>
#include <list>
#include <string>

#include "ast/type/Type.h"
#include "enums.h"
#include "utils/constexpr_utils.h"

namespace hzcc::ast {

NumericalType::NumericalType(PrimitiveType type)  // NOLINT
    : Type(TypeCategory::Numerical), _type(type) {}

std::string NumericalType::to_str() const {
    static constexpr int KPermittedTypeTableSize = 12;
    static constexpr std::array<const char*, KPermittedTypeTableSize>
        kPrimitiveTypeTable{"int",        "char",      "float",
                            "double",     "void",      "long",
                            "short",      "_Bool",     "_Complex",
                            "_Imaginary", "long long", "long double"};
    return kPrimitiveTypeTable[magic_enum::enum_integer(_type)];
}

bool NumericalType::is_same(const Type& rhs) const {
    if (rhs.is<TypeCategory::Numerical>()) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
        return GetTypeId() ==
               static_cast<const NumericalType*>(&rhs)->GetTypeId();
#pragma clang diagnostic pop
    }
    return false;
}
}  // namespace hzcc::ast