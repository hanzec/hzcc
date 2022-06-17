//
// Created by chen_ on 2022/3/26.
//
#include <utility>

#include "Type.h"

#ifndef HZCC_AST_TYPE_FUNC_PTR_TYPE_H
#define HZCC_AST_TYPE_FUNC_PTR_TYPE_H
namespace Hzcc::AST {
static std::string ToString(const std::list<Type::TypePtr>& types) {
    std::string ret = "(";
    for (const auto& type : types) {
        ret += type->Name() + ",";
    }
    return ret.replace(ret.size() - 1, 1, ")");
}

class FuncPtrType : public Type {
  public:
    FuncPtrType(std::shared_ptr<Type>& return_type,
                std::list<std::shared_ptr<Type>> args)
        : Type({}), _args(std::move(args)), _return_type(return_type) {}

    [[nodiscard]] bool IsFuncPtr() const override { return true; }

  private:
    std::shared_ptr<Type> _return_type;
    std::list<std::shared_ptr<Type>> _args;
};

}  // namespace Hzcc::AST
#endif  // HZCC_AST_TYPE_FUNC_PTR_TYPE_H
