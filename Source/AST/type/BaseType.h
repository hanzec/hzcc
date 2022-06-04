//
// Created by chen_ on 2022/5/11.
//
#include <bitset>
#include <string>
#ifndef HZCC_AST_TYPE_BASE_TYPE_H
#define HZCC_AST_TYPE_BASE_TYPE_H
namespace Hzcc::AST {
enum PrimitiveType {
    kPrimitiveType_int = 0,
    kPrimitiveType_float,
    kPrimitiveType_double,
    kPrimitiveType_char,
    kPrimitiveType_void,
    kPrimitiveType_long,
    kPrimitiveType_short,
    kPrimitiveType_ENUM_SIZE,  // must be the last one
};

class BaseType {
  public:
    explicit BaseType(std::string name);

    [[nodiscard]] std::string Dump() const;

    [[nodiscard]] const std::string& GetName() const;

    /**
     * @brief Check if the type is void
     * @return true if the type is void else false
     */
    [[nodiscard]] bool IsVoid() const { return _type[kPrimitiveType_void]; }

    /**
     * @brief Check if the type is character
     * @return True if the type is character, false otherwise.
     */
    [[nodiscard]] bool IsChar() const { return _type[kPrimitiveType_char]; }

    /**
     * @brief Check if the type is Float
     * @return True if the type is Float, false otherwise.
     */
    [[nodiscard]] bool IsFloat() const { return _type[kPrimitiveType_float]; }

    /**
     * @brief Check if the type is Double
     * @return True if the type is Double, false otherwise.
     */
    [[nodiscard]] bool IsDouble() const { return _type[kPrimitiveType_double]; }

    /**
     * @brief Check if the type is Integer
     * @return True if the type is Integer, false otherwise.
     */
    [[nodiscard]] bool IsInteger() const { return _type[kPrimitiveType_int]; }

    /**
     * @brief Check if the type is Long
     * @return True if the type is Long, false otherwise.
     */
    [[nodiscard]] bool IsLong() const { return _type[kPrimitiveType_long]; }

    /**
     * @brief Check if the type is Short
     * @return True if the type is Short, false otherwise.
     */
    [[nodiscard]] bool IsShort() const { return _type[kPrimitiveType_short]; }

    /**
     * @brief The base type id
     * @return uint64_t The base type id
     */
    [[nodiscard]] uint32_t GetTypeId() const { return _id; }

  private:
    uint32_t _id;
    std::string _name;
    inline static uint64_t _counter_ = 0;
    std::bitset<kPrimitiveType_ENUM_SIZE> _type;
};
}  // namespace Hzcc::AST

#endif  // HZCC_AST_TYPE_BASE_TYPE_H
