//
// Created by chen_ on 2022/4/10.
//
#include "Generator.h"

#include <filesystem>
#include <utility>
namespace Mycc::Codegen {

Generator::Generator(std::string output_file, std::string input_file)
    : _file_handler(output_file),
      _input_file_name(std::move(input_file)),
      _output_file_name(std::move(output_file)) {}

std::ostream& Generator::GetOstream() {
    static std::ofstream file = std::ofstream(_output_file_name);
    if (!_file_handler.bad()) {
        _file_handler << file.rdbuf();
    }
    return _file_handler;
}

void Generator::RedirectOutputStream(std::ostream& out) {
    _file_handler << out.rdbuf();
}
const std::string& Generator::GetInputFile() const { return _input_file_name; }
}  // namespace Mycc::Codegen