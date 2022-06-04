#include <bitset>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "BaseType.h"
#include "lexical/token_type.h"
#include "syntax/utils/token_utils.h"
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
     * @brief Deconstructor of RetType.
     * @Note: When a Type is deconstructed, if this RetType is last one created,
     * id of this RetType will be reused.
     */
    virtual ~Type();

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

    virtual std::string Dump();

    /**
     * @brief Check if the type is a constant value.
     * @return
     */
    [[nodiscard]] bool IsConst() const;

    /**
     * @brief Check if the type is void
     * @return True if the type is void, false otherwise.
     */
    [[nodiscard]] bool IsVoid() const { return _base_type->IsVoid(); }

    /**
     * @brief Check if the type is character
     * @return True if the type is character, false otherwise.
     */
    [[nodiscard]] bool IsChar() const { return _base_type->IsChar(); }

    /**
     * @brief Check if the type is Float
     * @return True if the type is Float, false otherwise.
     */
    [[nodiscard]] bool IsFloat() const { return _base_type->IsFloat(); }

    /**
     * @brief Check if the type is Double
     * @return True if the type is Double, false otherwise.
     */
    [[nodiscard]] bool IsDouble() const { return _base_type->IsDouble(); }

    /**
     * @brief Check if the type is Integer
     * @return True if the type is Integer, false otherwise.
     */
    [[nodiscard]] bool IsInteger() const { return _base_type->IsInteger(); }

    /**
     * @brief Check if the type is Long
     * @return True if the type is Long, false otherwise.
     */
    [[nodiscard]] bool IsLong() const { return _base_type->IsLong(); }

    /**
     * @brief Check if the type is Short
     * @return True if the type is Short, false otherwise.
     */
    [[nodiscard]] bool IsShort() const { return _base_type->IsShort(); }

    /**
     * @brief Get the declare name of the type.
     * @param without_attr If true, the attribute will not be included in the
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::string GetName(bool without_attr = false) const;

    /**
     * @brief Return the same type of this type but with const attribute.
     * @note For the type already has const attribute, this function will return
     * copy of self.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::shared_ptr<Type> GetConstType();

    std::list<Lexical::TokenType> GetAttributes();
    /**
     * @brief Get the basic type from its name.
     * @param name The name of the basic type.
     * @param attrs The attributes of the basic type.
     */
    static std::shared_ptr<Type> GetTypeOf(
        const std::string& name, const std::list<Lexical::TokenType>& attrs);

    /**
     * @brief Get the basic type from its name.
     * @param name The name of the basic type.
     * @param attrs The attributes of the basic type.
     */
    static std::shared_ptr<Type> GetTypeOf(
        const std::shared_ptr<Type>& other_type,
        const std::list<Lexical::TokenType>& attrs);

    // overload operator == for RetType
    bool operator==(const Type& type) const;

    // overload operator != for RetType
    bool operator!=(const Type& type) const;

  protected:
    using BaseTypePtr = std::shared_ptr<BaseType>;

    /**
     * @brief Get the basic type from its name.
     * @param name The name of the basic type.
     * @param attrs The attributes of the basic type.
     */
    static BaseTypePtr GetBaseType(const std::string& name);

    /**
     * @brief Constructor of RetType.
     * @param base_type The base type of the type.
     * @param attrs The attributes of the type.
     */
    Type(BaseTypePtr base_type, const std::list<Lexical::TokenType>& attrs);

    [[nodiscard]] uint64_t GetTypeId() const { return _id; }

    inline static std::unordered_map<std::string, TypePtr> _cached_types;
    inline static std::unordered_map<std::string, BaseTypePtr> _cached_b_types;

    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] virtual bool IsSame(const Type& rhs) const;

  private:
    uint64_t _id;
    BaseTypePtr _base_type;
    inline static uint64_t _counter_ = 0;
    std::bitset<kVariableAttrTableSize> _attrs;
};

}  // namespace Hzcc::AST

#endif  // HZCC_AST_TYPE_TYPE_H
