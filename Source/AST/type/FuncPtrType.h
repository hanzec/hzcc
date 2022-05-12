//
// Created by chen_ on 2022/3/26.
//
#include "Type.h"

#ifndef MYCC_SOURCE_AST_TYPE_FUNC_PTR_TYPE_H_
#define MYCC_SOURCE_AST_TYPE_FUNC_PTR_TYPE_H_
namespace Hzcc::AST {
static std::string ToString(const std::list<Type::TypePtr>& types) {
    std::string ret = "(";
    for (const auto& type : types) {
        ret += type->GetName() + ",";
    }
    return ret.replace(ret.size() - 1, 1, ")");
}

class FuncPtrType : public Type {
  public:
    FuncPtrType(std::shared_ptr<Type>& return_type,
                const std::list<std::shared_ptr<Type>>& args)
        : Type(Type::GetBaseType(return_type->GetName() + ToString(args)), {}),
          _args(args) {}

    [[nodiscard]] bool IsFuncPtr() const override { return true; }

  private:
    std::list<std::shared_ptr<Type>> _args;
};

}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_TYPE_FUNC_PTR_TYPE_H_
