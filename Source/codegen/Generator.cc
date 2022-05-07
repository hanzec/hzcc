//
// Created by chen_ on 2022/5/6.
//
#include "Generator.h"

namespace Hzcc::Codegen {
Generator::Generator(const std::string& output, AST::CompilationUnit& unit)
    : _current_unit(unit), _output_file(output) {}
const std::filesystem::path& Generator::GetOutputFilePath() const {
    return _output_file;
}
AST::CompilationUnit& Generator::GetCompilationUnit() { return _current_unit; }
}  // namespace Hzcc::Codegen