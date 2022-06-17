//
// Created by chen_ on 2022/6/14.
//
#include "Type.h"
#ifndef MYCC_SOURCE_AST_TYPE_POINTERTYPE_H_
#define MYCC_SOURCE_AST_TYPE_POINTERTYPE_H_
namespace Hzcc::AST {
class PointerType : Type {
  public:
    /**
     * @brief Construct a new PointerType object.
     *
     * @param base_type The base type of the pointer.
     * @param attrs The attributes of the pointer.
     */
    explicit PointerType(std::shared_ptr<Type> base_type,
                         const std::list<TokenType>& attrs = {});

    /**
     * @brief Check if the type is a pointer type
     * @return True if the type is a pointer type, false otherwise.
     */
    [[nodiscard]] bool IsPointer() const override;

    /**
     * @brief Get the base type of the pointer type.
     * @return std::shared_ptr<Type> The base type of the pointer type.
     */
    [[nodiscard]] std::shared_ptr<Type> GetBaseType() const;
  private:
    std::shared_ptr<Type> _base_type;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_TYPE_POINTERTYPE_H_
