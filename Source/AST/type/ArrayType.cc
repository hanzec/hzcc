//
// Created by chen_ on 2022/4/9.
//
#include "ArrayType.h"

#include <cassert>

#include "AST/ASTNode.h"
#include "AST/DeduceValue.h"
namespace Hzcc::AST {
std::shared_ptr<Type> ArrayType::GetArrayOfBasicType(
    const std::shared_ptr<Type>& type, std::unique_ptr<ASTNode> size,
    const std::list<Lexical::TokenType>& attrs) {
    // shared helper function
    struct make_shared_enabler : public ArrayType {
        explicit make_shared_enabler(const std::shared_ptr<Type>& base_type,
                                     std::unique_ptr<ASTNode> array_size,
                                     const std::list<Lexical::TokenType>& attrs)
            : ArrayType(base_type, std::move(array_size), attrs) {}
    };

    auto final_type =
        std::make_shared<make_shared_enabler>(type, std::move(size), attrs);

    if (_cached_types.find(final_type->GetName()) != _cached_types.end()) {
        return _cached_types[final_type->GetName()];
    } else {
        _cached_types[final_type->GetName()] = final_type;
        return final_type;
    }
}

ArrayType::ArrayType(const std::shared_ptr<Type>& base_type,
                     std::unique_ptr<ASTNode> array_size,
                     const std::list<Lexical::TokenType>& attrs)
    : Type(Type::GetBaseType(
               base_type->GetName() +
               (array_size == nullptr
                    ? "[]"
                    : (array_size->GetDeducedValue().has_value()
                           ? "[" +
                                 std::to_string(array_size->GetDeducedValue()
                                                    .value()
                                                    .AsInt()) +
                                 "]"
                           : "[VLA]"))),
           {}),
      _size_node(std::move(array_size)),
      _base_type(base_type) {}

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
std::unique_ptr<AST::ASTNode>& ArrayType::GetArraySizeNode() {
    return _size_node;
}

}  // namespace Hzcc::AST
