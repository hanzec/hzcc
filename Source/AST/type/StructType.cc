//
// Created by chen_ on 2022/4/9.
//

#include "StructType.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
StructType::StructType(const std::string &name,
                       const std::list<TokenType> &attr_list)
    : Type(attr_list), _name(name) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(!name.empty())
        << HZCC_AST_PRINT_TYPE_INFO("cast expr is nullptr", this);
}

bool StructType::IsStruct() const { return true; }

bool StructType::IsSame(const Type &rhs) const {
    if (rhs.IsStruct()) {
        const auto &rhs_struct = dynamic_cast<const StructType &>(rhs);
        return _name == rhs_struct._name;
    }
    return false;
}

bool StructType::AddChild(const std::shared_ptr<StructType> &type) {
    if (type == nullptr) {
        return false;
    }
    _localTypeList.emplace_back(type->Name(), type, std::list<TokenType>());
    return true;
}
bool StructType::AddChild(
    const std::string &name,                       // NOLINT
    const std::shared_ptr<Type> &type,             // NOLINT
    const std::list<Lexical::Token> &attr_list) {  // NOLINT
    if (type == nullptr) {
        return false;
    }
    // convert attr_list to TokenType
    std::list<TokenType> attr_list_type;
    for (auto &attr : attr_list) {
        attr_list_type.emplace_back(attr.Type());
    }

    _localTypeList.emplace_back(name, type, std::move(attr_list_type));
    return true;
}
std::shared_ptr<Type> StructType::ChildType(const std::string &name) {
    for (auto &item : _localTypeList) {
        if (std::get<0>(item) == name) {
            return std::get<1>(item);
        }
    }
    return nullptr;
}
std::string StructType::Name() const { return "struct " + _name; }
}  // namespace Hzcc::AST