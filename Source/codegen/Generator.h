//
// Created by chen_ on 2022/4/10.
//

#ifndef MYCC_SOURCE_CODEGEN_GENERATOR_H_
#define MYCC_SOURCE_CODEGEN_GENERATOR_H_
#include <memory>
#include <string>
#include <utility>

#include "utils/Status.h"
namespace Hzcc {
namespace AST {
class CompilationUnit;
}
namespace Codegen {
class Generator {
  public:
    /**
     * @brief Generate the code for the given compilation unit.
     * @param input_file  The input file.
     * @param output_file The output file to write the generated code to.
     * @param unit      The compilation unit to generate code for.
     * @return Status   The status of the generation.
     */
    virtual Status Generate(
        const std::string& input_file,                           // NOLINT
        const std::string& output_file,                          // NOLINT
        const std::unique_ptr<AST::CompilationUnit>& unit) = 0;  // NOLINT
};
}  // namespace Codegen
}  // namespace Hzcc
#endif  // MYCC_SOURCE_CODEGEN_GENERATOR_H_
