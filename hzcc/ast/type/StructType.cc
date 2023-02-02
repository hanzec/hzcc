//
// Created by chen_ on 2022/4/9.
//

#include <glog/logging.h>

#include <list>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include "ast/type/Type.h"
#include "enums.h"
#include "utils/logging.h"

namespace hzcc::ast {
StructType::StructType(std::string_view name)
    : IRecordType(TypeCategory::Struct), _name(name) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, !name.empty())
        << UniqueName() << "cast cast is nullptr";
#endif
}

bool StructType::is_same(const Type &rhs) const {
    if (rhs.is<TypeCategory::Struct>()) {
        const auto &rhs_struct = dynamic_cast<const StructType &>(rhs);
        return _name == rhs_struct._name;
    }
    return false;
}

void StructType::add_record(std::string_view name,      // NOLINT
                            const QualTypePtr &type) {  // NOLINT
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, !name.empty())  // NOLINT
        << UniqueName() << "to_str is empty";
    INTERNAL_LOG_IF(FATAL, type != nullptr)  // NOLINT
        << UniqueName() << "type is nullptr";
#endif
    _localTypeList.emplace_back(name, type);
}

QualTypePtr StructType::field_type(std::string_view name) {
    for (auto &item : _localTypeList) {
        if (std::get<0>(item) == name) {
            return std::get<1>(item);
        }
    }
    return nullptr;
}
std::string StructType::to_str() const { return "struct " + _name; }
}  // namespace hzcc::ast