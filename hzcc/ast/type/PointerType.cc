//
// Created by chen_ on 2022/6/14.
//

#include "ast/type/Type.h"
namespace hzcc::ast {

PointerType::PointerType(std::shared_ptr<Type> base_type,
                         const std::list<Attribute>& attrs)
    : Type(TypeCategory::kPointer, attrs),
      _base_type(std::move(base_type)) {}

bool PointerType::is_ptr() const { return true; }

std::shared_ptr<Type> PointerType::GetBaseType() const { return _base_type; }
}  // namespace hzcc::ast