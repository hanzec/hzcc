//
// Created by chen_ on 2022/4/10.
//
#include <memory>
#include <string>

#include "utils/logging.h"
#ifndef HZCC_CODEGEN_JVM_UTILS_TYPE_UTILS_H
#define HZCC_CODEGEN_JVM_UTILS_TYPE_UTILS_H

namespace Hzcc {
namespace AST {
class Type;
}
namespace Codegen::Utils {

std::string RemoveAttribute(const std::string &typename_);

std::string GetTypeName(const std::shared_ptr<AST::Type> &type,
                        bool replace_char = false);

}  // namespace Codegen::Utils
}  // namespace Hzcc

#endif  // HZCC_CODEGEN_JVM_UTILS_TYPE_UTILS_H
