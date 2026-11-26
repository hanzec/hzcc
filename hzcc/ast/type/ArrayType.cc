//
// Created by chen_ on 2022/4/9.
//
#include <glog/logging.h>
#include <stdint.h>

#include <list>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <utility>

#include "ast/DeduceValue.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "enums.h"

namespace hzcc::ast {
ArrayType::ArrayType(std::unique_ptr<Expr> array_size,
                     std::shared_ptr<QualType> base_type)
    : Type(TypeCategory::Array),
      _size_node(std::move(array_size)),
      _base_type(std::move(base_type)) {}

uint64_t ArrayType::GetSize() const {
    const auto size = _size_node->GetDeducedValue();
    return size ? size->AsInt() : 0;
}

bool ArrayType::HasDeduceSize() {
    return _size_node->GetDeducedValue().has_value();
}

bool ArrayType::is_same(const Type& type) const {
    if (type.is<TypeCategory::Array>()) {
        auto other = dynamic_cast<const ArrayType*>(&type);

        DCHECK(other != nullptr)
            << "dynamic cast to array type failed when is_arr() is true";

        return GetBaseType()->is_same_qualified(*other->GetBaseType());
    } else {
        return false;
    }
}
const std::unique_ptr<ast::Expr>& ArrayType::GetArraySizeNode() const {
    return _size_node;
}
std::string ArrayType::to_str() const {
    if (_size_node->stmt_type() == StmtType::EMPTY ||
        dynamic_cast<const EmptyExpr*>(_size_node.get())) {
        return _base_type->to_str() + "[]";
    }
    const auto size = _size_node->GetDeducedValue();
    return _base_type->to_str() +
           (size ? "[" + std::to_string(size->AsInt()) + "]" : "[VLA]");
}
}  // namespace hzcc::ast
