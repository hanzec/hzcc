//
// Created by chen_ on 2022/3/26.
//
#include "AST/type.h"

#ifndef MYCC_SOURCE_AST_TYPE_FUNC_PTR_TYPE_H_
#define MYCC_SOURCE_AST_TYPE_FUNC_PTR_TYPE_H_
namespace Mycc::AST {
class FuncPtrType: public Type {
  public:
    explicit FuncPtrType(const std::string& name) : Type(name) {}

    [[nodiscard]] bool IsFuncPtr() const override { return true; }
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_TYPE_FUNC_PTR_TYPE_H_
