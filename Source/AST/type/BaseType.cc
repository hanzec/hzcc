//
// Created by chen_ on 2022/5/11.
//

#include "BaseType.h"

#include <utility>

#include "utils/logging.h"
namespace Hzcc::AST {
BaseType::BaseType(std::string name)
    : _name(std::move(name)), _id(++_counter_) {
    DLOG_ASSERT(!(_name.find(' ') != std::string::npos &&
                  _name.find("struct") == std::string::npos))
        << "type name cannot contain space";

    switch (_name[0]) {
        case 'i':
            _type[kPrimitiveType_int] = true;
            break;
        case 'f':
            _type[kPrimitiveType_float] = true;
            break;
        case 'd':
            _type[kPrimitiveType_double] = true;
            break;
        case 'c':
            _type[kPrimitiveType_char] = true;
            break;
        case 'v':
            _type[kPrimitiveType_void] = true;
            break;
        default:
            DLOG(FATAL) << "unknown type name: " << _name;
    }
}

const std::string& BaseType::GetName() const { return _name; }

std::string BaseType::Dump() const {
    return _name + "(" + std::to_string(_id) + ")";
}

}  // namespace Hzcc::AST
