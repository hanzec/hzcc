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
#include "ast/Stmt.h"
#include "ast/type/Type.h"
#include "enums.h"

namespace hzcc::ast {
ArrayType::ArrayType(std::unique_ptr<Expr> array_size,
                     std::shared_ptr<QualType> base_type)
    : Type(TypeCategory::Array),
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

bool ArrayType::is_same(const Type& type) const {
    if (type.is<TypeCategory::Array>()) {
        auto other = dynamic_cast<const ArrayType*>(&type);

        DLOG_ASSERT(other != nullptr)
            << "dynamic cast to array type failed when is_arr() is true";

        return GetBaseType() == other->GetBaseType();
    } else {
        return false;
    }
}
const std::unique_ptr<ast::Expr>& ArrayType::GetArraySizeNode() const {
    return _size_node;
}
std::string ArrayType::to_str() const {
    return _base_type->to_str() +
           (_size_node->stmt_type() == StmtType::EMPTY
                ? "[]"
                : (_size_node->GetDeducedValue().has_value()
                       ? "[" +
                             std::to_string(_size_node->GetDeducedValue()
                                                .value()
                                                .AsInt()) +
                             "]"
                       : "[VLA]"));
}
}  // namespace hzcc::ast
