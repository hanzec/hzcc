//
// Created by chen_ on 2022/3/27.
//
#include <utility>

#include "Type.h"
#include "lexical/Token.h"
#ifndef HZCC_AST_TYPE_STRUCT_TYPE_H
#define HZCC_AST_TYPE_STRUCT_TYPE_H
namespace Hzcc::AST {
class StructType : public Type {
  public:
    StructType(const std::string &name,                      // NOLINT
               const std::list<TokenType> &attr_list = {});  // NOLINT

    /**
     * @brief Check if the type is a Struct.
     * @return True if the type is a Struct, false otherwise.
     */
    [[nodiscard]] bool IsStruct() const override;

    /**
     * @brief Get the declare name of the type.
     * @param without_attr If true, the attribute will not be included in the
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::string Name() const override;

    bool AddChild(const std::shared_ptr<StructType> &type);

    bool AddChild(const std::string &name,                           // NOLINT
                  const std::shared_ptr<Type> &type,                 // NOLINT
                  const std::list<Lexical::Token> &attr_list = {});  // NOLINT

    std::shared_ptr<Type> ChildType(const std::string &name);

  protected:
    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] bool IsSame(const Type& rhs) const override;

  private:
    const std::string _name;
    std::list<
        std::tuple<std::string, std::shared_ptr<Type>, std::list<TokenType>>>
        _localTypeList;
};

}  // namespace Hzcc::AST
#endif  // HZCC_AST_TYPE_STRUCT_TYPE_H
