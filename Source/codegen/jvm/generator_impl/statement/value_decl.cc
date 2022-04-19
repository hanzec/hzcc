//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/value_decl.h"

#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"
namespace Mycc::Codegen {
bool JVMGenerator::InitSource() {
    auto& output_file = GetOstream();

    // write public header
    output_file << ".class public "
                << GetInputFile().substr(0, GetInputFile().find_last_of('.'))
                << "\n";
    output_file << ".super java/lang/Object\n";
    return false;
}

int JVMGenerator::PushReturnStack() { return 0; }

int JVMGenerator::ConsumeReturnStack() { return 0; }

std::pair<int, char> JVMGenerator::GetStack(std::string name) {
    return std::pair<int, char>();
}

int JVMGenerator::BindStack(std::string name, char type, bool is_local) {
    return 0;
}

}  // namespace Mycc::Codegen
