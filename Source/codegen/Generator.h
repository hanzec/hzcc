//
// Created by chen_ on 2022/4/10.
//
#include <filesystem>
#include <string>

#include "utils/Status.h"
#ifndef MYCC_SOURCE_CODEGEN_GENERATOR_H_
#define MYCC_SOURCE_CODEGEN_GENERATOR_H_
namespace Hzcc {
namespace AST {
class CompilationUnit;
}
namespace Codegen {
class Generator {
  public:
    Generator(const std::string& output, std::shared_ptr<AST::CompilationUnit> unit);
    Generator();
    /**
     * @brief Generate the code for the given compilation unit.
     * @param input_file  The input file.
     * @param out The output file to write the generated code to.
     * @param unit      The compilation unit to generate code for.
     * @return Status   The status of the generation.
     */
    virtual Status Generate() = 0;  // NOLINT
  protected:

    std::shared_ptr<AST::CompilationUnit> GetCompilationUnit();

    [[nodiscard]] const std::filesystem::path& GetOutputFilePath() const;

  private:
    std::filesystem::path _output_file;
    std::shared_ptr<AST::CompilationUnit> _current_unit;
};
}  // namespace Codegen
}  // namespace Hzcc
#endif  // MYCC_SOURCE_CODEGEN_GENERATOR_H_
