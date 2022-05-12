//
// Created by chen_ on 2022/5/6.
//
#include "Generator.h"

namespace Hzcc::Codegen {
Generator::Generator(const std::string& output, std::shared_ptr<AST::CompilationUnit> unit)
    : _current_unit(unit), _output_file(output) {}
const std::filesystem::path& Generator::GetOutputFilePath() const {
    return _output_file;
}
std::shared_ptr<AST::CompilationUnit> Generator::GetCompilationUnit() { return _current_unit; }
Generator::Generator() {}
}  // namespace Hzcc::Codegen