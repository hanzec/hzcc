//
// Created by hanzech on 5/23/22.
//

#ifndef FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_INSTRUCTION_H_
#define FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_INSTRUCTION_H_
namespace Hzcc::Codegen::JVM {
/**
 * @brief Abstract JVM instruction with declared operand-stack consumption and production.
 */
class Instruction {
  public:
    /**
     * @brief Initialize Instruction with the supplied backend state.
     * @param consume_count Operand-stack slots consumed by this instruction.
     * @param push_count Operand-stack slots produced by this instruction.
     */
    Instruction(int consume_count, int push_count)
        : _consume_count(consume_count), _push_count(push_count){};

    /**
     * @brief Return the number of operand-stack slots produced by the instruction.
     * @return Number of produced slots.
     */
    [[nodiscard]] int PushCount() const { return _push_count; }

    /**
     * @brief Return the number of operand-stack slots consumed by the instruction.
     * @return Number of consumed slots.
     */
    [[nodiscard]] int ConsumeCount() const { return _consume_count; }

    /**
     * @brief Produce the textual JVM instruction sequence represented by this object.
     * @return Textual assembly for the instruction implemented by the derived class.
     */
    virtual std::string GenerateInstr() = 0;

  private:
    const int _push_count;
    const int _consume_count;
};
}  // namespace Hzcc::Codegen::JVM
#endif  // FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_INSTRUCTION_H_
