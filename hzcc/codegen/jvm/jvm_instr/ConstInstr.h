//
// Created by hanzech on 5/23/22.
//
#include "AST/DeduceValue.h"
#include "Instruction.h"
#include "codegen/jvm/Type.h"
#ifndef FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_CONSTINSTR_H_
#define FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_CONSTINSTR_H_
namespace Hzcc::Codegen::JVM {
/**
 * @brief Generate a constant-load instruction while borrowing its type and deduced value.
 */
class ConstInstr : public Instruction {
  public:
    /**
     * @brief Initialize ConstInstr with the supplied backend state.
     * @param type JVM type or descriptor used by this operation.
     * @param val Constant value to encode in the instruction sequence.
     * @note Both referenced arguments must outlive this instruction.
     */
    ConstInstr(const JVM::Type &type, const AST::DeduceValue &val);

    /**
     * @brief Produce the textual JVM instruction sequence represented by this object.
     * @return Assembly instructions that load the represented constant.
     */
    std::string GenerateInstr() override;

  private:
    const JVM::Type &_type;
    const AST::DeduceValue &_val;
};
}  // namespace Hzcc::Codegen::JVM
#endif  // FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_CONSTINSTR_H_
