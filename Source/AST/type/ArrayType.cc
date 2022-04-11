//
// Created by chen_ on 2022/4/9.
//
#include "ArrayType.h"

#include <glog/logging.h>

#include <cassert>

#include "AST/ASTNode.h"
#include "AST/DeduceValue.h"
namespace Mycc::AST {
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

    if (_types.find(final_type->GetName()) != _types.end()) {
        return _types[final_type->GetName()];
    } else {
        _types[final_type->GetName()] = final_type;
        return final_type;
    }
}

ArrayType::ArrayType(const std::shared_ptr<Type>& base_type,
                     std::unique_ptr<ASTNode> array_size,
                     const std::list<Lexical::TokenType>& attrs)
    : Type(base_type->GetName() +
               (array_size == nullptr
                    ? "[]"
                    : (array_size->IsDeducible()
                           ? "[" +
                                 std::to_string(array_size->GetDeducedValue()
                                                    .value()
                                                    .AsInt()) +
                                 "]"
                           : "[VLA]")),
           {}),
      _size_node(std::move(array_size)),
      _base_type(base_type) {}

uint64_t ArrayType::GetSize() const {
    return _size_node->IsDeducible() ? 0
                                     : _size_node->GetDeducedValue()->AsInt();
}
bool ArrayType::HasSize() { return _size_node != nullptr; }

bool ArrayType::IsSame(const std::shared_ptr<Type>& type) const {
    // check current type
    if (type == nullptr) {
        DLOG(FATAL) << "cannot compare with nullptr";
    }

    if (type->IsArray()) {
        auto other = std::dynamic_pointer_cast<ArrayType>(type);

        DLOG_ASSERT(other != nullptr)
            << "dynamic cast to array type failed when IsArray() is true";

        return GetBaseType()->IsSame(other->GetBaseType());
    } else {
        return false;
    }
}

}  // namespace Mycc::AST
