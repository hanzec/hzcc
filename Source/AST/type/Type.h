#include <bitset>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "utils/logging.h"
#ifndef HZCC_AST_TYPE_TYPE_H
#define HZCC_AST_TYPE_TYPE_H
namespace Hzcc::AST {
class ASTNode;
class BaseType;
/**
 * @class RetType
 * @brief RetType of a variable or a function.
 */
class Type : public std::enable_shared_from_this<Type> {
  public:
    using TypePtr = std::shared_ptr<Type>;

    /**
     * @brief Constructor of RetType.
     * @param base_type The base type of the type.
     * @param attrs The attributes of the type.
     */
    explicit Type(const std::list<TokenType>& attrs = {});

    virtual ~Type() = default;

    /**
     * @brief The unique id of the node.
     * @return The unique id of the node.
     */
    [[nodiscard]] uintptr_t Id() const;


    /**
     * @brief Check if the type is an Array.
     * @return True if the type is an Array, false otherwise.
     */
    [[nodiscard]] virtual bool IsArray() const;

    /**
     * @brief Check if the type is a Struct.
     * @return True if the type is a Struct, false otherwise.
     */
    [[nodiscard]] virtual bool IsStruct() const;

    /**
     * @brief Check if the type is a Function pointer type.
     * @return True if the type is a Function pointer type, false otherwise.
     */
    [[nodiscard]] virtual bool IsFuncPtr() const;

    /**
     * @brief Check if the type is a pointer type
     * @return True if the type is a pointer type, false otherwise.
     */
    [[nodiscard]] virtual bool IsPointer() const;

    /**
     * @brief Check if the type is a builtin type.
     * @return True if the type is a builtin type, false otherwise.
     */
    [[nodiscard]] virtual bool IsBuiltinType() const;

    virtual std::string Dump();

    /**
     * @brief Check if the type is a constant value.
     * @return
     */
    [[nodiscard]] bool IsConst() const;

    /**
     * @brief Get the declare name of the type.
     * @param without_attr If true, the attribute will not be included in the
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] virtual std::string Name() const = 0;

    std::list<TokenType> GetAttributes();

    // overload operator == for RetType
    bool operator==(const Type& type) const;

    // overload operator != for RetType
    bool operator!=(const Type& type) const;

  protected:
    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] virtual bool IsSame(const Type& rhs) const = 0;

  private:
    std::bitset<kAttributeTableSize> _attrs;
};

}  // namespace Hzcc::AST

#endif  // HZCC_AST_TYPE_TYPE_H
