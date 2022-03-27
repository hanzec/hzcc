//
// Created by chen_ on 2022/3/26.
//
#include "type.h"

#ifndef MYCC_SOURCE_AST_TYPE_FUNC_PTR_TYPE_H_
#define MYCC_SOURCE_AST_TYPE_FUNC_PTR_TYPE_H_
namespace Mycc::AST {
class FuncPtrType : public Type {
  public:
    FuncPtrType(std::shared_ptr<Type>& return_type,
                const std::list<std::shared_ptr<Type>>& args)
        : Type(return_type->GetName() + ToString(args)), _args(args) {}

    [[nodiscard]] bool IsFuncPtr() const override { return true; }

  private:
    std::list<std::shared_ptr<Type>> _args;
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_TYPE_FUNC_PTR_TYPE_H_
