//
// Created by chen_ on 2022/6/13.
//
#include <bitset>
#include <string>

#include "Type.h"
#ifndef MYCC_SOURCE_AST_TYPE_BUILDINTYPE_H_
#define MYCC_SOURCE_AST_TYPE_BUILDINTYPE_H_
namespace Hzcc::AST {
enum PrimitiveType {
    kPrimitiveType_void = 0,
    kPrimitiveType_char,
    kPrimitiveType_short,
    kPrimitiveType_int,
    kPrimitiveType_long,
    kPrimitiveType_longlong,
    kPrimitiveType_float,
    kPrimitiveType_double,
    kPrimitiveType_long_double,
    kPrimitiveType_ENUM_SIZE,  // must be the last one
};

/**
 * @brief The BuiltinType class
 * @details This class is used for builtin types like 'int'.
 */
class BuiltinType : public Type {
  public:
    explicit BuiltinType(PrimitiveType type,                           // NOLINT
                         const std::list<TokenType>& attr_list = {});  // NOLINT

    /**
     * @brief Check if the type is void
     * @return True if the type is void, false otherwise.
     */
    [[nodiscard]] bool IsVoid() const;

    /**
     * @brief Check if the type is Long
     * @return True if the type is Long, false otherwise.
     */
    [[nodiscard]] bool IsLong() const;

    /**
     * @brief Check if the type is Short
     * @return True if the type is Short, false otherwise.
     */
    [[nodiscard]] bool IsShort() const;

    /**
     * @brief Check if the type is character
     * @return True if the type is character, false otherwise.
     */
    [[nodiscard]] bool IsChar() const;

    /**
     * @brief Check if the type is Float
     * @return True if the type is Float, false otherwise.
     */
    [[nodiscard]] bool IsFloat() const;

    /**
     * @brief Check if the type is Double
     * @return True if the type is Double, false otherwise.
     */
    [[nodiscard]] bool IsDouble() const;

    /**
     * @brief Check if the type is Integer
     * @return True if the type is Integer, false otherwise.
     */
    [[nodiscard]] bool IsInteger() const;

    /**
     * @brief Check if the type is longlong
     * @return True if the type is longlong, false otherwise.
     */
    [[nodiscard]] bool IsLongLong() const;

    /**
     * @brief Check if the type is long double
     * @return True if the type is long double, false otherwise.
     */
    [[nodiscard]] bool IsLongDouble() const;

    [[nodiscard]] uint8_t GetTypeId() const;

    /**
     * @brief Get the declare name of the type.
     * @param without_attr If true, the attribute will not be included in the
     * name.
     * @return std::string The declare name of the type.
     */
    [[nodiscard]] std::string Name() const override;

    [[nodiscard]] bool IsBuiltinType() const override;

  protected:
    /**
     * @brief Check if the type is the same as another type.
     * @return True if the type is the same as another type, false otherwise.
     */
    [[nodiscard]] bool IsSame(const Type& rhs) const override;

  private:
    const PrimitiveType _type;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_TYPE_BUILDINTYPE_H_
