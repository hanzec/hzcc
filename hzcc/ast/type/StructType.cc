//
// Created by chen_ on 2022/4/9.
//

#include "ast/type/Type.h"
#include "utils/logging.h"
namespace hzcc::ast {
StructType::StructType(const std::string &name,
                       const std::list<Attribute> &attr_list)
    : Type(TypeCategory::kStruct, attr_list), _name(name) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, !name.empty())
        << UniqueName() << "cast cast is nullptr";
#endif
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
    _localTypeList.emplace_back(type->Name(), type);
    return true;
}
bool StructType::AddChild(const std::string &name,              // NOLINT
                          const std::shared_ptr<Type> &type) {  // NOLINT
    if (type == nullptr) {
        return false;
    }
    _localTypeList.emplace_back(name, type);
    return true;
}
std::shared_ptr<Type> StructType::field_type(std::string_view name) {
    for (auto &item : _localTypeList) {
        if (std::get<0>(item) == name) {
            return std::get<1>(item);
        }
    }
    return nullptr;
}
std::string StructType::Name() const { return "struct " + _name; }
}  // namespace hzcc::ast