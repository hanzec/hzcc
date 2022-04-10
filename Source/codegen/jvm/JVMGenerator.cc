//
// Created by chen_ on 2022/4/10.
//

#include "JVMGenerator.h"

#include <gflags/gflags.h>

#include <fstream>

#include "const_code_block.h"
#include "utils/logging.h"
namespace Mycc::Codegen {
JVMGenerator::JVMGenerator(const std::string &output_name,
                           const std::string &input_name)
    : Generator(output_name, input_name) {}

bool JVMGenerator::FinalizedSource() {
    auto &file_handler = GetOstream();

    // write special method to f
    file_handler << kConstSpecialMethod;

    return true;
}


void JVMGenerator::IncLindeIndent() { _indent += _indent_str; }

void JVMGenerator::DecLindeIndent() {
    _indent = _indent.substr(0, _indent.size() - 4);
}

const std::string &JVMGenerator::GetLineIndent() const { return _indent; }

}  // namespace Mycc::Codegen