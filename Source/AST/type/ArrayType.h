//
// Created by chen_ on 2022/3/26.
//
#include "Type.h"
#ifndef HZCC_AST_TYPE_ARRAY_TYPE_H
#define HZCC_AST_TYPE_ARRAY_TYPE_H
namespace Hzcc::AST {
class ArrayType : public Type {
  public:
    ArrayType(std::shared_ptr<Type> base_type,
              std::unique_ptr<ASTNode> array_size,
              const std::list<TokenType>& attrs = {});

    [[nodiscard]] bool IsArray() const override;

    [[nodiscard]] std::shared_ptr<AST::Type> GetBaseType() const {
        return _base_type;
    }

    [[nodiscard]] bool HasDeduceSize();

    [[nodiscard]] uint64_t GetSize() const;

    [[nodiscard]] const std::unique_ptr<AST::ASTNode>& GetArraySizeNode() const;

    [[nodiscard]] bool IsSame(const Type& type) const override;

    /**
     * @brief Get the declare name of the type.
     * @param without_attr If true, the attribute will not be included in the
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::string Name() const override;

  private:
    const std::shared_ptr<AST::Type> _base_type;
    const std::unique_ptr<AST::ASTNode> _size_node;
};

}  // namespace Hzcc::AST
#endif  // HZCC_AST_TYPE_ARRAY_TYPE_H
