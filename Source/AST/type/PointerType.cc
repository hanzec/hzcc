//
// Created by chen_ on 2022/6/14.
//

#include "PointerType.h"
namespace Hzcc::AST {

PointerType::PointerType(std::shared_ptr<Type> base_type,
                         const std::list<TokenType>& attrs)
    : Type(attrs), _base_type(std::move(base_type)) {}

bool PointerType::IsPointer() const { return true; }

std::shared_ptr<Type> PointerType::GetBaseType() const { return _base_type; }
}  // namespace Hzcc::AST