//
// Created by chen_ on 2022/3/27.
//
#include <utility>

#include "Type.h"

#ifndef MYCC_SOURCE_AST_TYPE_STRUCT_TYPE_H_
#define MYCC_SOURCE_AST_TYPE_STRUCT_TYPE_H_
namespace Mycc::AST {
class StructType : public Type {
  public:
    [[nodiscard]] bool IsStruct() const override { return true; }

    bool AddChild(const std::shared_ptr<StructType> &type) {
        if (type == nullptr) {
            return false;
        }
        _localTypeList.emplace_back(type->GetName(), type,
                                    std::list<Lexical::TokenType>());
        return true;
    }

    bool AddChild(const std::string &name, std::shared_ptr<Type> type,
                  std::list<Lexical::TokenType> &attr_list) {
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

  protected:
    friend class ASTContext;
    friend class SymbolTable;
    explicit StructType(const std::string &name,
                        const std::list<Lexical::TokenType> &attr_list)
        : Type("struct " + name, attr_list) {}

  private:
    std::list<std::tuple<std::string, std::shared_ptr<Type>,
                         std::list<Lexical::TokenType>>>
        _localTypeList;
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_TYPE_STRUCT_TYPE_H_