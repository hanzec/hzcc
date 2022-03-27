//
// Created by chen_ on 2022/3/27.
//
#include "type.h"

#include "utils/logging.h"
namespace Mycc::AST {

std::string Type::ToString(const std::list<std::shared_ptr<Type>>& types) {
    std::string ret = "(";
    for (const auto& type : types) {
        ret += type->GetName() + ",";
    }
    return ret.replace(ret.size() - 1, 1, ")");
}
Type::Type(const std::string& name, const std::list<std::string>& attrs)
    : _name(name), _id(++_counter_), _is_const(false) {
    // check type name
    if (name.find(' ') != std::string::npos &&
        name.find("struct") == std::string::npos) {
        DLOG(ERROR) << "type name cannot contain space";
    }

    // check attrs
    for (const auto& attr : attrs) {
        if (attr.find(' ') != std::string::npos) {
            DLOG(ERROR) << "type attr cannot contain space";
        }
        if (attr == "const") {
            if (_is_const) {
                DLOG(ERROR) << "type cannot be const twice";
            }
            _is_const = true;
        }
    }
}
}  // namespace Mycc::AST