//
// Created by chen_ on 2022/3/29.
//
#include <array>
#include <cstddef>
#ifndef MYCC_SOURCE_AST_VALUE_H_
#define MYCC_SOURCE_AST_VALUE_H_
namespace Mycc::AST {
class DeduceValue {
  public:
    enum Type {
        kInt,
        kReal_Number,
    };

    explicit DeduceValue(double value);

    explicit DeduceValue(uint64_t value);

    DeduceValue(Type type, std::string str_value);

    [[nodiscard]] int64_t AsInt() const;
    [[nodiscard]] double AsReal() const;

    [[nodiscard]] Type GetType() const;

  private:
    Type _type;
    int64_t _raw_value = 0;
};

DeduceValue operator+(DeduceValue i, const DeduceValue& n);

DeduceValue operator-(DeduceValue i, const DeduceValue& n);

DeduceValue operator*(DeduceValue i, const DeduceValue& n);

DeduceValue operator/(DeduceValue i, const DeduceValue& n);

DeduceValue operator%(DeduceValue i, const DeduceValue& n);
}  // namespace Mycc::AST

#endif  // MYCC_SOURCE_AST_VALUE_H_
