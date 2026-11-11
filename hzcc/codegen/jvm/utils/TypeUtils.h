//
// Created by chen_ on 2022/4/10.
//
#include <memory>
#include <string>

#include "utils/logging.h"
#ifndef HZCC_CODEGEN_JVM_UTILS_TYPE_UTILS_H
#define HZCC_CODEGEN_JVM_UTILS_TYPE_UTILS_H

namespace hzcc {
namespace AST {
/**
 * @brief Represent a legacy AST type in JVM descriptors and instruction type prefixes.
 */
class Type;
}
namespace Codegen::Utils {

/**
 * @brief Declare normalization of legacy AST type spellings.
 * @param typename_ Type spelling intended for normalization.
 * @return Intended normalized spelling.
 * @note This legacy utility declaration has no matching definition in its namespace.
 */
std::string RemoveAttribute(const std::string &typename_);

/**
 * @brief Declare conversion of an AST type to a JVM descriptor; the legacy body is unfinished.
 * @param type JVM type or descriptor used by this operation.
 * @param replace_char Use the integer representation for character values when true.
 * @return Intended JVM type descriptor; no usable result is currently implemented.
 * @note This legacy helper is incomplete and is not enabled by the current build.
 */
std::string GetTypeName(const std::shared_ptr<AST::Type> &type,
                        bool replace_char = false);

}  // namespace Codegen::Utils
}  // namespace hzcc

#endif  // HZCC_CODEGEN_JVM_UTILS_TYPE_UTILS_H
