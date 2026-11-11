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
     * @brief Construct a new Deduce to_str object.
     * @param value value of the deduce value as character.
     */
    explicit DeduceValue(char value);

    /**
     * @brief Construct a new Deduce to_str object.
     * @param value value of the deduce value in real number
     */
    explicit DeduceValue(double value);

    /**
     * @brief Construct a new Deduce to_str object.
     * @param value value of the deduce value in integer
     */
    explicit DeduceValue(int64_t value);

    /**
     * @brief Construct a new Deduce to_str object.
     * @param type type of the deduce value
     * @param str_value Literal spelling parsed according to type.
     */
    DeduceValue(DeduceValueType type, std::string str_value);

    /**
     * @brief Get the value of the deduce value as integer number.
     * @details If the type is not kDeduceValueType_Char, will trigger an assert
     * error if compile as DEBUG. Undefined behavior if compile as NDEBUG.
     * @return the integer value of the deduce value
     */
    [[nodiscard]] char as_char() const;

    /**
     * @brief Get the value of the deduce value as integer number.
     * @details The value conversion will based on the following rules:
     * - If the value is real number, the value will be truncated toward zero.
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
 * @brief Add two compile-time values.
 * @param lhs Left operand copied for evaluation.
 * @param rhs Borrowed right operand.
 * @return An integer when both operands are integers or both are characters;
 *         otherwise a real-valued result.
 */
DeduceValue operator+(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Subtract the right compile-time value from the left.
 * @param lhs Left operand copied for evaluation.
 * @param rhs Borrowed right operand.
 * @return An integer when both operands are integers or both are characters;
 *         otherwise a real-valued result.
 */
DeduceValue operator-(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Multiply two compile-time values.
 * @param lhs Left operand copied for evaluation.
 * @param rhs Borrowed right operand.
 * @return An integer when both operands are integers or both are characters;
 *         otherwise a real-valued result.
 */
DeduceValue operator*(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Divide the left compile-time value by the right.
 * @param lhs Left operand copied for evaluation.
 * @param rhs Borrowed right operand.
 * @return An integer when both operands are integers or both are characters;
 *         otherwise a real-valued result.
 * @pre Integer division requires a nonzero divisor and a representable quotient.
 */
DeduceValue operator/(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Evaluate the legacy percent operation on integer-converted values.
 * @param lhs Left operand copied for evaluation.
 * @param rhs Borrowed right operand.
 * @return The integer quotient produced by the current implementation.
 * @note This implementation currently divides rather than computing a remainder.
 * @pre The integer-converted right operand is nonzero.
 */
DeduceValue operator%(DeduceValue lhs, const DeduceValue& rhs);
}  // namespace hzcc::ast

#endif  // HZCC_AST_DEDUCE_VALUE_H
