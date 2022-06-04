//
// Created by hanzech on 5/23/22.
//
#include "AST/DeduceValue.h"
#include "Instruction.h"
#include "codegen/jvm/Type.h"
#ifndef FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_CONSTINSTR_H_
#define FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_CONSTINSTR_H_
namespace Hzcc::Codegen::JVM {
class ConstInstr : public Instruction {
  public:
    ConstInstr(const JVM::Type &type, const AST::DeduceValue &val);

    std::string GenerateInstr() override;
  private:
    const JVM::Type &_type;
    const AST::DeduceValue &_val;
};
}  // namespace Hzcc::Codegen::JVM
#endif  // FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_CONSTINSTR_H_
