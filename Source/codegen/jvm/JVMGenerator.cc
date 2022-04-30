//
// Created by chen_ on 2022/4/10.
//

#include "JVMGenerator.h"

#include <glog/logging.h>

#include <filesystem>
#include <fstream>

#include "const_code_block.h"
#include "utils/logging.h"
#include "version.h"
namespace Mycc::Codegen {
Status JVMGenerator::Generate(
    const std::string &intput, const std::string &output,
    const std::unique_ptr<AST::CompilationUnit> &unit) {
    DVLOG_IF(CODE_GEN_LEVEL, !output.empty())
        << "JVM code generator not support assign output file, output file "
           "name assignment will be ignored";

    // output file name will be replaced by the input file with .class
    std::filesystem::path input_path(intput);
    std::filesystem::path output_path(input_path.replace_extension(".class"));
    DVLOG(CODE_GEN_LEVEL) << "input file: " << intput
                          << ", output file: " << output_path;

    // generate class file
    std::ofstream output_file(output_path);

    // write class file header
    output_file << ";; Generated by Mycc" << FULL_VERSION << "\n"
                << ";; From Language: c\n"
                << ";; Source Path: " << intput << "\n";

    // write class file body
    output_file << ".class public " << input_path.replace_extension().filename()
                << "\n"
                << ".super java/lang/Object\n";

    // generate class body
    auto ret = VisitAllAST(unit);
    if (!ret.Ok()) {
        return std::move(ret);
    }

    // write class file tail
    output_file << kConstSpecialMethod;
    output_file.close();

    return Status::OkStatus();
}

void JVMGenerator::IncLindeIndent() { _indent += _indent_str; }

void JVMGenerator::DecLindeIndent() {
    _indent = _indent.substr(0, _indent.size() - 4);
}

const std::string &JVMGenerator::GetLineIndent() const { return _indent; }

int JVMGenerator::PushReturnStack() { return 0; }

int JVMGenerator::ConsumeReturnStack() { return 0; }

std::pair<int, char> JVMGenerator::GetStack(std::string name) {
    return std::pair<int, char>(0, 0);
}

int JVMGenerator::BindStack(std::string name, char type, bool is_local) {
    return 0;
}

}  // namespace Mycc::Codegen
