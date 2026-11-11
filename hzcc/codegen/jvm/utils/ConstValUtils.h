//
// Created by chen_ on 2022/5/3.
//
#include <string>

#include "AST/DeduceValue.h"

#ifndef HZCC_CODEGEN_JVM_UTILS_CONST_VAL_UTILS_H
#define HZCC_CODEGEN_JVM_UTILS_CONST_VAL_UTILS_H
namespace hzcc::Codegen::Utils {

/**
 * @brief Select JVM load-constant assembly for the supplied literal value.
 * @param val Constant value to encode in the instruction sequence.
 * @return Assembly text for the constant; incomplete overloads have no valid result contract.
 * @note This legacy helper is incomplete and is not enabled by the current build.
 */
std::string PushConstVal(char val);

/**
 * @brief Select JVM load-constant assembly for the supplied literal value.
 * @param val Constant value to encode in the instruction sequence.
 * @return Assembly text for the constant; incomplete overloads have no valid result contract.
 * @note This legacy helper is incomplete and is not enabled by the current build.
 */
std::string PushConstVal(long val);

/**
 * @brief Select JVM load-constant assembly for the supplied literal value.
 * @param val Constant value to encode in the instruction sequence.
 * @return Assembly text for the constant; incomplete overloads have no valid result contract.
 * @note This legacy helper is incomplete and is not enabled by the current build.
 */
std::string PushConstVal(float val);

/**
 * @brief Select JVM load-constant assembly for the supplied literal value.
 * @param val Constant value to encode in the instruction sequence.
 * @return Assembly text for the constant; incomplete overloads have no valid result contract.
 * @note This legacy helper is incomplete and is not enabled by the current build.
 */
std::string PushConstVal(double val);

/**
 * @brief Select JVM load-constant assembly for the supplied literal value.
 * @param val Constant value to encode in the instruction sequence.
 * @return Assembly text for the constant; incomplete overloads have no valid result contract.
 * @note This legacy helper is incomplete and is not enabled by the current build.
 */
std::string PushConstVal(int32_t val);

/**
 * @brief Select JVM load-constant assembly for the supplied literal value.
 * @param type JVM type or descriptor used by this operation.
 * @param val Constant value to encode in the instruction sequence.
 * @return Assembly text for the constant; incomplete overloads have no valid result contract.
 * @note This legacy helper is incomplete and is not enabled by the current build.
 */
std::string PushConstVal(std::string const &type, const AST::DeduceValue &val);
}  // namespace hzcc::Codegen::Utils
#endif  // HZCC_CODEGEN_JVM_UTILS_CONST_VAL_UTILS_H
