//
// Created by chen_ on 2022/6/14.
//

#include <list>
#include <memory>
#include <utility>

#include "ast/type/Type.h"
#include "enums.h"

namespace hzcc::ast {

PointerType::PointerType(QualTypePtr base_type)
    : Type(TypeCategory::Ptr), _base_type(std::move(base_type)) {}

bool PointerType::is_same(const Type& rhs) const {
    if (rhs.is<TypeCategory::Ptr>()) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
        return base_type() ==
               static_cast<const PointerType*>(&rhs)->base_type();
#pragma clang diagnostic pop
    }
    return false;
}
std::string PointerType::to_str() const { return std::string(); }
}  // namespace hzcc::ast