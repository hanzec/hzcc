//
// Created by chen_ on 2022/5/3.
//
#include <string>

#include "AST/DeduceValue.h"

#ifndef HZCC_CODEGEN_JVM_UTILS_CONST_VAL_UTILS_H
#define HZCC_CODEGEN_JVM_UTILS_CONST_VAL_UTILS_H
namespace hzcc::Codegen::Utils {

std::string PushConstVal(char val);

std::string PushConstVal(long val);

std::string PushConstVal(float val);

std::string PushConstVal(double val);

std::string PushConstVal(int32_t val);

std::string PushConstVal(std::string const &type, const AST::DeduceValue &val);
}  // namespace hzcc::Codegen::Utils
#endif  // HZCC_CODEGEN_JVM_UTILS_CONST_VAL_UTILS_H
