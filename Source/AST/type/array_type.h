//
// Created by chen_ on 2022/3/26.
//
#include "type.h"
#ifndef MYCC_SOURCE_AST_TYPE_ARRAY_TYPE_H_
#define MYCC_SOURCE_AST_TYPE_ARRAY_TYPE_H_
namespace Mycc::AST {
class ArrayType : public Type {
  public:
    explicit ArrayType(const std::shared_ptr<Type>& base_type)
        : Type(base_type->GetName() + "[]"), _size(-1), _base_type(base_type) {}

    ArrayType(const std::shared_ptr<Type>& base_type,
              std::unique_ptr<AST::ASTNode> size)
        : Type(base_type->GetName() + "[VLA]"),
          _size(-1),
          _size_node(std::move(size)) {}

    ArrayType(const std::shared_ptr<Type>& base_type, int64_t size)
        : Type(base_type->GetName() + "[" + std::to_string(size) + "]"),
          _size(size),
          _base_type(base_type) {}

    [[nodiscard]] bool IsArray() const override { return true; }

    [[nodiscard]] std::shared_ptr<AST::Type> GetBaseType() const {
        return _base_type;
    }

    [[nodiscard]] bool HasSize() const {
        return _size != -1 || _size_node != nullptr;
    }

    [[nodiscard]] int64_t GetSize() const { return _size; }

  private:
    const int64_t _size;
    std::shared_ptr<AST::Type> _base_type;
    const std::unique_ptr<AST::ASTNode> _size_node{nullptr};
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_TYPE_ARRAY_TYPE_H_
