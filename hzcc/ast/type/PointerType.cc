//
// Created by chen_ on 2022/6/14.
//

#include <list>
#include <memory>
#include <utility>

#include "ast/type/Type.h"
#include "enums.h"

namespace hzcc::ast {

PointerType::PointerType(std::shared_ptr<Type> base_type,
                         const std::list<Attribute>& attrs)
    : Type(TypeCategory::kPointer, attrs), _base_type(std::move(base_type)) {}

bool PointerType::is_same(const Type& rhs) const {
    if (rhs.is_ptr()) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
        return GetBaseType() ==
               static_cast<const PointerType*>(&rhs)->GetBaseType();
#pragma clang diagnostic pop
    }
    return false;
}
std::string PointerType::Name() const { return std::string(); }
}  // namespace hzcc::ast