//
// Created by chen_ on 2022/4/10.
//

#include "TypeUtils.h"

#include <cstring>
#include <string>

#include "AST/type/ArrayType.h"
#include "AST/type/Type.h"
#include "lexical/keywords.h"
namespace Hzcc::Codegen::Utils {

/**
 * @brief Declare conversion of an AST type to a JVM descriptor; the legacy body is unfinished.
 * @param type JVM type or descriptor used by this operation.
 * @param replace_char Use the integer representation for character values when true.
 * @return Intended JVM type descriptor; no usable result is currently implemented.
 * @note This legacy helper is incomplete and is not enabled by the current build.
 */
std::string GetTypeName(const std::shared_ptr<AST::Type> &type,
                        bool replace_char) {}
}  // namespace Hzcc::Codegen::Utils
