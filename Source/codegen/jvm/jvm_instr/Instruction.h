//
// Created by hanzech on 5/23/22.
//

#ifndef FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_INSTRUCTION_H_
#define FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_INSTRUCTION_H_
namespace Hzcc::Codegen::JVM {
class Instruction {
  public:
    Instruction(int consume_count, int push_count)
        : _consume_count(consume_count), _push_count(push_count){};

    [[nodiscard]] int PushCount() const { return _push_count; }

    [[nodiscard]] int ConsumeCount() const { return _consume_count; }

    virtual std::string GenerateInstr() = 0;

  private:
    const int _push_count;
    const int _consume_count;
};
}  // namespace Hzcc::Codegen::JVM
#endif  // FCALL3_CLASS_SOURCE_CODEGEN_JVM_JVM_INSTR_INSTRUCTION_H_
