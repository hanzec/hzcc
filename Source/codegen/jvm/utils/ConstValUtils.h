//
// Created by chen_ on 2022/5/3.
//
#include <string>

#include "AST/DeduceValue.h"

#ifndef MYCC_SOURCE_CODEGEN_JVM_UTILS_CONSTVALUTILS_H_
#define MYCC_SOURCE_CODEGEN_JVM_UTILS_CONSTVALUTILS_H_
namespace Hzcc::Codegen::Utils {

std::string PushConstVal(char val);

std::string PushConstVal(long val);

std::string PushConstVal(float val);

std::string PushConstVal(double val);

std::string PushConstVal(int32_t val);

std::string PushConstVal(std::string const &type, const AST::DeduceValue &val);
}  // namespace Hzcc::Codegen::Utils
#endif  // MYCC_SOURCE_CODEGEN_JVM_UTILS_CONSTVALUTILS_H_
