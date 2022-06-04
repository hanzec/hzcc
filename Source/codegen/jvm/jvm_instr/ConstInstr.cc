//
// Created by hanzech on 5/23/22.
//

#include "ConstInstr.h"
namespace Hzcc::Codegen::JVM {
constexpr static std::array<const char *, 7> kConstIntASM = {
    "iconst_M1", "iconst_0", "iconst_1", "iconst_2",
    "iconst_3",  "iconst_4", "iconst_5"};
constexpr static std::array<const char *, 3> kConstFloatASM = {
    "fconst_0", "fconst_1", "fconst_2"};
constexpr static std::array<const char *, 3> kConstDoubleASM = {"dconst_0",
                                                                "dconst_1"};
constexpr static std::array<const char *, 2> kConstLongASM = {"lconst_0",
                                                              "lconst_1"};

ConstInstr::ConstInstr(const JVM::Type &type, const Hzcc::AST::DeduceValue &val)
    : Instruction(0, 1), _val(val), _type(type) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(type.IsLong() || type.IsInt() || type.IsFloat() ||
                type.IsShort() || type.IsChar() || type.IsDouble())
        << "Instruction only accept primitive type: integer, float, double, "
           "long, short, char";
}

std::string ConstInstr::GenerateInstr() {
    // if integer type
    if (_type.IsInt() || _type.IsLong() || _type.IsShort() || _type.IsLong()) {
        // extract the value
        auto val = _val.AsInt();

        // generate the instruction
        if (_type.IsLong() && (val == 0 || val == 1)) {
            return kConstLongASM[val];
        }

        // push int value use const int
        else if (_type.IsInt() && (val >= -1 && val <= 5)) {
            return kConstIntASM[val + 1];
        }

        // else push with bipush/sipush/ldc
        else {
            // if its between 127 and -128, we can use bipush
            if (val >= -128 && val <= 127)
                return std::string("bipush ") + std::to_string(val);

            // if its between 32767 and -32768, we can use sipush
            else if (val >= -32768 && val <= 32767)
                return std::string("sipush ") + std::to_string(val);

            // else use ldc
            else
                return std::string("ldc ") + std::to_string(val);
        }
    }

    // if float/double type
    else {
        // extract the value
        auto val = _val.AsReal();

        // single instr to generate float value
        if (_type.IsFloat() && (val == 0.0f || val == 1.0f || val == 2.0f)) {
            return kConstFloatASM[static_cast<int>(val)];
        }

        // double instr to generate double value
        else if (_type.IsDouble() && (val == 0.0 || val == 1.0)) {
            return kConstDoubleASM[static_cast<int>(val)];
        }

        // else use ldc
        else {
            return std::string("ldc ") + std::to_string(val);
        }
    }
}
}  // namespace Hzcc::Codegen::JVM
