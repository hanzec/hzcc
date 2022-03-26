//
// Created by chen_ on 2022/3/26.
//
#include "AST/type.h"
#ifndef MYCC_SOURCE_AST_TYPE_ARRAY_TYPE_H_
#define MYCC_SOURCE_AST_TYPE_ARRAY_TYPE_H_
namespace Mycc::AST {
class ArrayType: public Type {
  public:
    explicit ArrayType(const std::string& name) : Type(name) {}

    [[nodiscard]] bool IsArray() const override { return true; }
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_TYPE_ARRAY_TYPE_H_
