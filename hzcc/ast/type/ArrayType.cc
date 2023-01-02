//
// Created by chen_ on 2022/4/9.
//
#include <glog/logging.h>
#include <stdint.h>
#include <utility>
#include <list>
#include <memory>
#include <optional>
#include <ostream>
#include <string>

#include "ast/DeduceValue.h"
#include "ast/type/Type.h"
#include "ast/Stmt.h"
#include "enums.h"

namespace hzcc::ast {
ArrayType::ArrayType(std::shared_ptr<Type> base_type,
                     std::unique_ptr<Expr> array_size,
                     const std::list<Attribute>& attrs)
    : Type(TypeCategory::kArray, attrs),
      _size_node(std::move(array_size)),
      _base_type(std::move(base_type)) {}

uint64_t ArrayType::GetSize() const {
    return _size_node->GetDeducedValue().has_value()
               ? _size_node->GetDeducedValue()->AsInt()
               : 0;
}

bool ArrayType::HasDeduceSize() {
    return _size_node->GetDeducedValue().has_value();
}

bool ArrayType::IsSame(const Type& type) const {
    if (type.IsArray()) {
        auto other = dynamic_cast<const ArrayType*>(&type);

        DLOG_ASSERT(other != nullptr)
            << "dynamic cast to array type failed when IsArray() is true";

        return *GetBaseType() == *other->GetBaseType();
    } else {
        return false;
    }
}
const std::unique_ptr<ast::Expr>& ArrayType::GetArraySizeNode() const {
    return _size_node;
}
std::string ArrayType::Name() const {
    return _base_type->Name() +
           (_size_node->IsEmptyStmt()
                ? "[]"
                : (_size_node->GetDeducedValue().has_value()
                       ? "[" +
                             std::to_string(_size_node->GetDeducedValue()
                                                .value()
                                                .AsInt()) +
                             "]"
                       : "[VLA]"));
}
bool ArrayType::IsArray() const { return true; }

}  // namespace hzcc::ast
