//
// Created by chen_ on 2022/3/27.
//
#include <utility>

#include "type.h"

#ifndef MYCC_SOURCE_AST_TYPE_STRUCT_TYPE_H_
#define MYCC_SOURCE_AST_TYPE_STRUCT_TYPE_H_
namespace Mycc::AST {
class StructType : public Type {
  public:
    explicit StructType(const std::string &name,
                        std::list<std::string> attr_list)
        : Type("struct " + name), _attr_list(std::move(attr_list)) {}

    [[nodiscard]] bool IsStruct() const override { return true; }

    std::list<std::string> GetAttribute() { return _attr_list; }

    bool AddChild(std::shared_ptr<StructType> type) {
        if (type == nullptr) {
            return false;
        }
        _localTypeList.emplace_back(type->GetName(), std::move(type),
                                    type->GetAttribute());
        return true;
    }

    bool AddChild(const std::string &name, std::shared_ptr<Type> type,
                  std::list<std::string> &attr_list) {
        if (type == nullptr) {
            return false;
        }
        _localTypeList.emplace_back(name, std::move(type),
                                    std::move(attr_list));
        return true;
    }

    std::shared_ptr<Type> GetChild(const std::string &name) {
        for (auto &item : _localTypeList) {
            if (std::get<0>(item) == name) {
                return std::get<1>(item);
            }
        }
        return nullptr;
    }

  private:
    const std::list<std::string> _attr_list;
    std::list<
        std::tuple<std::string, std::shared_ptr<Type>, std::list<std::string>>>
        _localTypeList;
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_TYPE_STRUCT_TYPE_H_
