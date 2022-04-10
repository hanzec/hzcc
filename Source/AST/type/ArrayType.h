//
// Created by chen_ on 2022/3/26.
//
#include "Type.h"
#ifndef MYCC_SOURCE_AST_TYPE_ARRAY_TYPE_H_
#define MYCC_SOURCE_AST_TYPE_ARRAY_TYPE_H_
namespace Mycc::AST {
class ArrayType : public Type {
  public:
    [[nodiscard]] bool IsArray() const override { return true; }

    [[nodiscard]] std::shared_ptr<AST::Type> GetBaseType() const {
        return _base_type;
    }

    [[nodiscard]] bool HasSize();

    [[nodiscard]] uint64_t GetSize() const;

    [[nodiscard]] bool IsSame(const std::shared_ptr<Type>& type) const override;

    /**
     * @brief Get the basic type from its name.
     * @param name The name of the basic type.
     * @param attrs The attributes of the basic type.
     */
    static std::shared_ptr<Type> GetArrayOfBasicType(
        const std::shared_ptr<Type>& type, std::unique_ptr<ASTNode> size,
        const std::list<Lexical::TokenType>& attrs);

  protected:
    ArrayType(const std::shared_ptr<Type>& base_type,
              std::unique_ptr<AST::ASTNode>,
              const std::list<Lexical::TokenType>& attrs);

  private:
    std::shared_ptr<AST::Type> _base_type;
    const std::unique_ptr<AST::ASTNode> _size_node{nullptr};
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_TYPE_ARRAY_TYPE_H_
