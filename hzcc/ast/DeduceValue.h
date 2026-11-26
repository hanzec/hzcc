//
// Created by chen_ on 2022/3/29.
//
#include <cstdint>
#include <string>
#include <variant>

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
     * @brief Read the stored character.
     * @throws std::bad_variant_access if the value is not a character.
     * @return The character value.
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
    std::variant<char, int64_t, double> _value;
};

/**
 * @brief Add two compile-time values.
 * @param lhs Left operand copied for evaluation.
 * @param rhs Borrowed right operand.
 * @return An integer when both operands have integral values;
 *         otherwise a real-valued result.
 */
DeduceValue operator+(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Subtract the right compile-time value from the left.
 * @param lhs Left operand copied for evaluation.
 * @param rhs Borrowed right operand.
 * @return An integer when both operands have integral values;
 *         otherwise a real-valued result.
 */
DeduceValue operator-(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Multiply two compile-time values.
 * @param lhs Left operand copied for evaluation.
 * @param rhs Borrowed right operand.
 * @return An integer when both operands have integral values;
 *         otherwise a real-valued result.
 */
DeduceValue operator*(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Divide the left compile-time value by the right.
 * @param lhs Left operand copied for evaluation.
 * @param rhs Borrowed right operand.
 * @return An integer when both operands have integral values;
 *         otherwise a real-valued result.
 * @pre Integer division requires a nonzero divisor and a representable quotient.
 */
DeduceValue operator/(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Compute the remainder of two integral values.
 * @param lhs Left operand copied for evaluation.
 * @param rhs Borrowed right operand.
 * @return The integer remainder with the sign of the dividend.
 * @pre Both operands are integral, the divisor is nonzero and the quotient fits.
 */
DeduceValue operator%(DeduceValue lhs, const DeduceValue& rhs);
}  // namespace hzcc::ast

#endif  // HZCC_AST_DEDUCE_VALUE_H
