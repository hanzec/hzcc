//
// Created by chen_ on 2022/3/29.
//
#include <array>
#include <cstdint>
#include <string>
#include <cstddef>

#ifndef HZCC_AST_DEDUCE_VALUE_H
#define HZCC_AST_DEDUCE_VALUE_H
namespace hzcc::ast {
/**
 * @brief The type of the value, currently only integer, real number and
 * character literal are supported.
 */
enum DeduceValueType {
    kDeduceValueType_Int = 0,
    kDeduceValueType_Char = 1,
    kDeduceValueType_Real_Number = 3,
    kDeduceValueType_ENUM_SIZE = 4  // keep last
};

/**
 * @brief The base class of all deducible value. The deducible value is the
 * value we can calculate during the compile.
 */
class DeduceValue {
  public:
    /**
     * @brief Construct a new Deduce val object.
     * @param value value of the deduce value as character.
     */
    explicit DeduceValue(char value);

    /**
     * @brief Construct a new Deduce val object.
     * @param value value of the deduce value in real number
     */
    explicit DeduceValue(double value);

    /**
     * @brief Construct a new Deduce val object.
     * @param value value of the deduce value in integer
     */
    explicit DeduceValue(int64_t value);

    /**
     * @brief Construct a new Deduce val object.
     * @param type type of the deduce value
     * @param value value of the deduce value in string format
     */
    DeduceValue(DeduceValueType type, std::string str_value);

    /**
     * @brief Get the value of the deduce value as integer number.
     * @details If the type is not kDeduceValueType_Char, will trigger an assert
     * error if compile as DEBUG. Undefined behavior if compile as NDEBUG.
     * @return the integer value of the deduce value
     */
    [[nodiscard]] char AsChar() const;

    /**
     * @brief Get the value of the deduce value as integer number.
     * @details The value conversion will based on the following rules:
     * - If the value is real number, the value will be rounded to integer.
     * - If the value is integer, the value will be returned as is.
     * @return the integer value of the deduce value
     */
    [[nodiscard]] int64_t AsInt() const;

    /**
     * @brief Get the value of the deduce value as real number.
     * @details The value conversion will based on the following rules:
     * - If the value is integer, the value will be converted to real number.
     * - If the value is real number, the value will be returned as is.
     * @return the real number value of the deduce value
     */
    [[nodiscard]] double AsReal() const;

    /**
     * @brief Get the type of the deduce value.
     * @return the type of the deduce value
     */
    [[nodiscard]] DeduceValueType GetType() const;

  private:
    DeduceValueType _type;
    std::array<std::byte, 8> _raw_value = {
        std::byte(0x0), std::byte(0x0), std::byte(0x0), std::byte(0x0),
        std::byte(0x0), std::byte(0x0), std::byte(0x0), std::byte(0x0)};
};

/**
 * @brief Overload the biop + for deduce value.
 * @details The transformation rules are as follows:
 *          |             | Integer     | Real Number |  Character  |
 *          | :---------: | :---------: | :---------: | :---------: |
 *          | Integer     | Integer     | Real Number |   Integer   |
 *          | Real Number | Real Number | Real Number | Real Number |
 *          | Character   | Integer     | Real Number |   Integer   |
 * @param lhs left hand side of the biop
 * @param rhs right hand side of the biop
 */
DeduceValue operator+(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Overload the biop - for deduce value.
 * @details The transformation rules are as follows:
 *          |             | Integer     | Real Number |  Character  |
 *          | :---------: | :---------: | :---------: | :---------: |
 *          | Integer     | Integer     | Real Number |   Integer   |
 *          | Real Number | Real Number | Real Number | Real Number |
 *          | Character   | Integer     | Real Number |   Integer   |
 * @param lhs left hand side of the biop
 * @param rhs right hand side of the biop
 */
DeduceValue operator-(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Overload the biop * for deduce value.
 * @details The transformation rules are as follows:
 *          |             | Integer     | Real Number |  Character  |
 *          | :---------: | :---------: | :---------: | :---------: |
 *          | Integer     | Integer     | Real Number |   Integer   |
 *          | Real Number | Real Number | Real Number | Real Number |
 *          | Character   | Integer     | Real Number |   Integer   |
 * @param lhs left hand side of the biop
 * @param rhs right hand side of the biop
 */
DeduceValue operator*(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Overload the biop / for deduce value.
 * @details The transformation rules are as follows:
 *          |             | Integer     | Real Number |  Character  |
 *          | :---------: | :---------: | :---------: | :---------: |
 *          | Integer     | Integer     | Real Number |   Integer   |
 *          | Real Number | Real Number | Real Number | Real Number |
 *          | Character   | Integer     | Real Number |   Integer   |
 * @param lhs left hand side of the biop
 * @param rhs right hand side of the biop
 */
DeduceValue operator/(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Overload the biop % for deduce value.
 * @details The transformation rules are as follows:
 *          |             | Integer     | Real Number |  Character  |
 *          | :---------: | :---------: | :---------: | :---------: |
 *          | Integer     | Integer     | Integer     | Integer     |
 *          | Real Number | Integer     | Integer     | Integer     |
 *          | Character   | Integer     | Integer     | Integer     |
 * @param lhs left hand side of the biop
 * @param rhs right hand side of the biop
 */
DeduceValue operator%(DeduceValue lhs, const DeduceValue& rhs);
}  // namespace hzcc::ast

#endif  // HZCC_AST_DEDUCE_VALUE_H
