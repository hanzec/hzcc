//
// Created by chen_ on 2022/4/10.
//
#include <filesystem>
#include <string>

#include "utils/status/status.h"
#ifndef HZCC_SOURCE_CODEGEN_GENERATOR_H
#define HZCC_SOURCE_CODEGEN_GENERATOR_H
namespace hzcc {
namespace AST {
/**
 * @brief Forward declaration of the compilation unit used by the legacy backend.
 */
class CompilationUnit;
}
namespace Codegen {
/**
 * @brief Base interface retaining the output path and shared compilation unit for code generation.
 */
class Generator {
  public:
    /**
     * @brief Initialize Generator with the supplied backend state.
     * @param output Requested output path.
     * @param unit Non-null compilation unit retained through shared ownership.
     */
    Generator(const std::string& output,
              std::shared_ptr<AST::CompilationUnit> unit);
    /**
     * @brief Initialize an empty Generator.
     */
    Generator();
    /**
     * @brief Generate output for the compilation unit retained by this generator.
     * @return OK on success, or the first backend/pass error.
     */
    virtual Status Generate() = 0;  // NOLINT
  protected:
    /**
     * @brief Return shared ownership of the compilation unit associated with this generator.
     * @return The retained shared compilation-unit handle.
     */
    std::shared_ptr<AST::CompilationUnit> GetCompilationUnit();

    /**
     * @brief Return the configured output path; the reference is owned by this generator.
     * @return A reference to the generator-owned output path.
     */
    [[nodiscard]] const std::filesystem::path& GetOutputFilePath() const;

  private:
    std::filesystem::path _output_file;
    std::shared_ptr<AST::CompilationUnit> _current_unit;
};
}  // namespace Codegen
}  // namespace hzcc
#endif  // HZCC_SOURCE_CODEGEN_GENERATOR_H
