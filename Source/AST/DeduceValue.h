//
// Created by chen_ on 2022/3/29.
//
#ifndef MYCC_SOURCE_AST_DEDUCEVALUE_H_
#define MYCC_SOURCE_AST_DEDUCEVALUE_H_
#include <string>
#include <cstdint>

namespace Mycc::AST {
/**
 * @brief The base class of all deducible value. The deducible value is the
 * value we can calculate during the compile.
 */
class DeduceValue {
  public:
    /**
     * @brief The type of the value, currently only integer and real number are
     * supported.
     */
    enum Type {
        kInt,
        kReal_Number,
    };

    /**
     * @brief Construct a new Deduce Value object.
     * @param value value of the deduce value in real number
     */
    explicit DeduceValue(double value);

    /**
     * @brief Construct a new Deduce Value object.
     * @param value value of the deduce value in integer
     */
    explicit DeduceValue(uint64_t value);

    /**
     * @brief Construct a new Deduce Value object.
     * @param type type of the deduce value
     * @param value value of the deduce value in string format
     */
    DeduceValue(Type type, std::string str_value);

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
    [[nodiscard]] Type GetType() const;

  private:
    Type _type;
    int64_t _raw_value = 0;
};

/**
 * @brief Overload the operator + for deduce value.
 * @details The transformation rules are as follows:
 *          |             | Integer     | Real Number |
 *          | :---------: | :---------: | :---------: |
 *          | Integer     | Integer     | Real Number |
 *          | Real Number | Real Number | Real Number |
 * @param lhs left hand side of the operator
 * @param rhs right hand side of the operator
 */
DeduceValue operator+(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Overload the operator - for deduce value.
 * @details The transformation rules are as follows:
 *          |             | Integer     | Real Number |
 *          | :---------: | :---------: | :---------: |
 *          | Integer     | Integer     | Real Number |
 *          | Real Number | Real Number | Real Number |
 * @param lhs left hand side of the operator
 * @param rhs right hand side of the operator
 */
DeduceValue operator-(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Overload the operator * for deduce value.
 * @details The transformation rules are as follows:
 *          |             | Integer     | Real Number |
 *          | :---------: | :---------: | :---------: |
 *          | Integer     | Integer     | Real Number |
 *          | Real Number | Real Number | Real Number |
 * @param lhs left hand side of the operator
 * @param rhs right hand side of the operator
 */
DeduceValue operator*(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Overload the operator / for deduce value.
 * @details The transformation rules are as follows:
 *          |             | Integer     | Real Number |
 *          | :---------: | :---------: | :---------: |
 *          | Integer     | Integer     | Real Number |
 *          | Real Number | Real Number | Real Number |
 * @param lhs left hand side of the operator
 * @param rhs right hand side of the operator
 */
DeduceValue operator/(DeduceValue lhs, const DeduceValue& rhs);

/**
 * @brief Overload the operator % for deduce value.
 * @details The transformation rules are as follows:
 *          |             | Integer     | Real Number |
 *          | :---------: | :---------: | :---------: |
 *          | Integer     | Integer     | Real Number |
 *          | Real Number | Real Number | Real Number |
 * @param lhs left hand side of the operator
 * @param rhs right hand side of the operator
 */
DeduceValue operator%(DeduceValue lhs, const DeduceValue& rhs);
}  // namespace Mycc::AST

#endif  // MYCC_SOURCE_AST_DEDUCEVALUE_H_
