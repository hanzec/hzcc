//
// Created by hanzech on 5/23/22.
//
#include <string>
#include <utility>

#include "Type.h"
#include "utils/logging.h"
#include "utils/status/status.h"
#ifndef HZCC_SOURCE_CODEGEN_JVM_JVM_CLASS_H
#define HZCC_SOURCE_CODEGEN_JVM_JVM_CLASS_H
namespace Hzcc::Codegen::JVM {
/**
 * @brief Legacy class-file assembly writer; integration with the current frontend is unfinished.
 */
class JVMClass {
  public:
    /**
     * @brief Initialize JVMClass with the supplied backend state.
     * @param class_name Name of the JVM class owning generated fields and methods.
     * @param source_file_name Original source filename recorded in the assembly.
     */
    JVMClass(std::string class_name, std::string source_file_name);

  protected:
    /**
     * @brief Placeholder for a JVM assembly code section; no section behavior is implemented yet.
     */
    class CodeSection {
      public:
        /**
         * @brief Initialize an empty CodeSection.
         */
        explicit CodeSection() = default;
    };

    /**
     * @brief Write the legacy class assembly skeleton to a previously unused path.
     * @param file_name Destination path; an existing file is rejected.
     * @return INVALID_ARGUMENT when the path already exists; otherwise the current body returns OK.
     * @note Stream creation and write failures are not checked by this unfinished implementation.
     */
    Status SaveToFile(const std::string &file_name) const;

    /**
     * @brief Declare a static field insertion interface; its implementation is not present.
     * @param name Variable or symbol name to register or look up.
     * @param type JVM type or descriptor used by this operation.
     * @note This interface is only declared in the legacy backend.
     */
    void AddStaticVariable(std::string name, Type type);

  private:
    const std::string _class_name;
    const std::string _source_file_name;
};
}  // namespace Hzcc::Codegen::JVM
#endif  // HZCC_SOURCE_CODEGEN_JVM_JVM_CLASS_H
