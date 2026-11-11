//
// Created by chen_ on 2022/5/6.
//
#include "Generator.h"

namespace Hzcc::Codegen {
/**
 * @brief Initialize Generator with the supplied backend state.
 * @param output Requested output path.
 * @param unit Non-null compilation unit retained through shared ownership.
 */
Generator::Generator(const std::string& output,
                     std::shared_ptr<AST::CompilationUnit> unit)
    : _current_unit(unit), _output_file(output) {}
/**
 * @brief Return the configured output path; the reference is owned by this generator.
 * @return A reference to the generator-owned output path.
 */
const std::filesystem::path& Generator::GetOutputFilePath() const {
    return _output_file;
}
/**
 * @brief Return shared ownership of the compilation unit associated with this generator.
 * @return The retained shared compilation-unit handle.
 */
std::shared_ptr<AST::CompilationUnit> Generator::GetCompilationUnit() {
    return _current_unit;
}
/**
 * @brief Initialize an empty Generator.
 */
Generator::Generator() {}
}  // namespace Hzcc::Codegen
