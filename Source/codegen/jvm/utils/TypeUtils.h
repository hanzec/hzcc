//
// Created by chen_ on 2022/4/10.
//

#ifndef MYCC_SOURCE_CODEGEN_JVM_UTILS_TYPEUTILS_H_
#define MYCC_SOURCE_CODEGEN_JVM_UTILS_TYPEUTILS_H_
#include <string>
namespace Mycc::Codegen::TypeUtils {

char GetJVMTypename(const std::string &typename_);

} // namespace Mycc::Codegen::TypeUtils
#endif  // MYCC_SOURCE_CODEGEN_JVM_UTILS_TYPEUTILS_H_
